/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see LICENSE for license information.
 *
 * Based on work by:
 * Author: Hongliang Gao;   Created: Jan 27 2011
 */
 
#define DUMP_CYCLE_INFO

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>
#include <inttypes.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <time.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;


#include "predictor.h"
#include "cbp3_def.h"
#include "cbp3_framework.h"

#include "trace_interface.h"
#include "cbp3_stats.h"


using namespace std;


Program_Graph *PG;
uint64_t num_run = 0;
void run( int sim_len, int period );
void reset();
cbp3_stats stats;


po::variables_map parse_options( int argc, char *argv[] ){
	po::options_description desc("Options");
	desc.add_options()
		("help,h", "produce help message")
		("trace,t", po::value< string >(), "input trace")
		("length,l", po::value<int>()->default_value(-1), "Number of uops to simulate. Negative values run whole trace")
		("period,p", po::value<int>()->default_value(-1), "Set the period to output stats. Negative values cause a single output at the end")
		("gzip,z", po::value<bool>()->implicit_value(true), "Required if the trace is compressed with gzip")
		("pred_opt0,r", po::value<int>()->default_value(-1), "Pass a value to the predictor")
		("pred_opt1,s", po::value<int>()->default_value(-1), "Pass a second value to the predictor")
		//("opts,o", po::value< string >(), "options to pass to the predictor")
		//("table_size,s", po::value<uint64_t>()->default_value(10), "log2(#table entries)")
		//("pc_bits,p", po::value<uint64_t>()->default_value(7), "# of pc bits to use in hash")
		//("use_global,g", po::value<bool>()->default_value(true), "only use/track global history")
		//("gzip,z", po::value<bool>(), "Read a trace compressed with gzip")
		//("gzip,z", po::value<bool>()->default_value(false), "Read a trace compressed with gzip")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    

	if (vm.count("help")) {
		cout << desc << "\n";
		exit(1);
	}

	if( ! vm.count("trace") ) {
		cout << "Requires input trace\n" << desc << "\n";
		exit(1);
	}

	return vm;
}


int main ( int argc, char *argv[] )
{
    //time_t time_start = time(NULL);

	po::variables_map vm = parse_options( argc, argv );

	string trace_file       = vm["trace"].as<string>();
	int sim_len				= vm["length"].as<int>();
	int period				= vm["period"].as<int>();

	int pred_opt0			= vm["pred_opt0"].as<int>();
	int pred_opt1		    = vm["pred_opt1"].as<int>();

	bool gzip;
	if( vm.count("gzip") )
		gzip = true;
	else
		gzip = false;


// load the trace into memory
    printf("********************  CBP3  Start ***********************\n");
    printf("Trace:            %s\n", trace_file.c_str() );
    if (sim_len == -1)
        printf("Uops to simulate: Whole Trace\n\n");
    else
        printf("Uops to simulate: %i\n\n", sim_len );


 	PG = new Program_Graph( trace_file, gzip );


	if( pred_opt1 >= 0 )
		PredictorInit( pred_opt0, pred_opt1 );
	else if( pred_opt0 >= 0 )
		PredictorInit( pred_opt0 );
	else
		PredictorInit();

	reset();
	printf("\n*********  RUN  %d   *********\n", (int) num_run);

	
	//chunk is the number of entries to pull into memory from the trace file
	int chunk = 10000;

	while( PG->parse_trace( chunk ) ){


        run( sim_len, period );

    }

	stats.print();
    PredictorRunEnd();

// the end...
    printf("\n********************  CBP3  End   ***********************\n");
    //printf("Total Simulation Time: %ld Seconds\n", (time(NULL) - time_start));
    PredictorExit();

    return 0;
}


// live uop info that will be availabe to the predictor
// depending on the stage of the uop, fields are copied from uop_info array
vector<cbp3_queue_entry_t*> fetch_uops;
vector<cbp3_queue_entry_t*> exe_uops;
vector<cbp3_queue_entry_t*> retire_uops;



bool     rewind_marked;
uint64_t cycle;
cbp3_cycle_activity_t cycle_info;

void reset() {

    rewind_marked = false;
    cycle = 0;

	stats.reset();

    PredictorReset();
}

void run( int sim_len, int period ) {
    uint16_t num_stages[NUM_STAGES];
    
    while (PG->Run_a_Cycle(num_stages) && // trace does not end
            (sim_len == -1 ? 1 : stats.get_num_uops() < (uint64_t)sim_len)) { // in the simulation range


        cycle = PG->get_cycle();
        cycle_info.cycle = cycle;
        cycle_info.num_fetch = num_stages[0];
        cycle_info.num_allocate = num_stages[1];
        cycle_info.num_exe = num_stages[2];
        cycle_info.num_retire = num_stages[3];
        cycle_info.num_agu = num_stages[4];
        cycle_info.num_std = num_stages[5];

        // fill in different fields into live_uop array for each stage

		PG->get_retire_uops( retire_uops );
		PG->get_fetch_uops( fetch_uops );
		PG->get_exe_uops( exe_uops );
		
		//retire stage
		/*
        for (uint64_t i = 0; i < cycle_info.num_retire; i ++) {
            cbp3_queue_entry_t *uop = retire_uops[i];
            uint16_t type = uop->uop.type;
        }
		*/

		//on exe update mispredict info

        for (uint64_t i = 0; i < cycle_info.num_exe; i ++) {
            cbp3_queue_entry_t *uop = exe_uops[i];

            assert(uop->valid);

            // calculate mispred info
            uint16_t type	= uop->uop.type;

			bool is_cond	= (type & IS_BR_CONDITIONAL);
			bool is_ind		= (type & IS_BR_INDIRECT);

            bool cond_msp	= (uop->uop.br_taken != (uop->taken_pred >= 0));
            bool ind_msp	= (uop->uop.br_target != uop->target_pred);

			if( is_cond && is_ind ){
            //cout << uop->uop.br_taken << " "<< uop->taken_pred << endl;
				//cout << "THIS: " << cond_msp << endl;
			}

			if (!uop->cycle_last_pred) {
				cond_msp = true;
				ind_msp = true;
			}


			//update( bool taken, bool cond, bool ind, bool cond_msp, bool ind_msp )
			stats.update( uop->uop.br_taken, is_cond, is_ind, cond_msp, ind_msp );


            uop->cycle_exe = cycle;
        }

        PredictorRunACycle();
        stats.total_cycle++;

		if( period > 0 ){
			if( (cycle%period) == 0 ){
				stats.print();
				stats.reset();
			}
		}
    };
}


const cbp3_cycle_activity_t *get_cycle_info() {
    return &cycle_info;
}

cbp3_queue_entry_t* get_fetch_entry(uint8_t e) {
    return fetch_uops[e];
}

cbp3_queue_entry_t* get_exe_entry(uint8_t e) {
    return exe_uops[e];
}


cbp3_queue_entry_t* get_retire_entry(uint8_t e) {
    return retire_uops[e];
}


bool  report_taken_pred( cbp3_queue_entry_t* uop, int64_t pred) {
	/*
	cout << pred << endl;
	cout << endl;
	*/

    if (!uop->valid)
        return false;
    
    // record the prediction
    uop->cycle_last_pred = cycle;
    uop->taken_pred = pred;
    return true;
}
bool  report_target_pred( cbp3_queue_entry_t* uop, uint64_t pred) {

    if (!uop->valid)
        return false;
    
    // record the prediction
    uop->cycle_last_pred = cycle;
    uop->target_pred = pred;
    return true;
}

void cbp3_queue_entry_t::reset() {
    uop.reset();
    valid = false;
    cycle_fetch = 0;
    cycle_allocate = 0;
    cycle_agu = 0;
    cycle_std = 0;
    cycle_exe = 0;
    cycle_retire = 0;
    cycle_last_pred = 0;
    taken_pred = 0;
    target_pred = 0;
}


