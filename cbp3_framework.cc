/*
 * Copyright (c) 2013 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
// Author: Hongliang Gao;   Created: Jan 27 2011
// Description: implementation of the framework (driver) for cbp3.
// DO NOT MODIFY. THIS FILE WILL NOT BE SUBMITTED WITH YOUR PREDICTOR
 
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

using namespace std;
#include "cbp3_def.h"
#include "cbp3_reader.h"
#include "cbp3_framework.h"
#include "predictor.h"

#include "program_graph.h"

uint64_t num_run = 0;
Program_Graph PG;
void run(int sim_len);


int main ( int argc, char *argv[] )
{
    time_t time_start = time(NULL);

// process command inputs
    char tfile[500];
    int sim_len = -1; // to the end of trace
    if ( argc != 3 && argc != 5) {
        printf( "usage: %s -t /path-to/trace [-u num-uops-to-simulate]\n", argv[0]);
        exit (1);
    }else {
        int targ = -1, uarg = -1;
        bool wrong = false;
        for (int i = 1; i <= argc - 2; i += 2) {
            if (strcmp(argv[i], "-t") == 0) targ = i + 1;
            else if (strcmp(argv[i], "-u") == 0) uarg = i + 1;
            else 
                wrong = true;
        }
        if (targ == -1 || wrong)  {
            printf( "usage: %s -t /path-to/trace [-u num-uops-to-simulate]\n", argv[0]);
            exit (1);
        }
        
        if (uarg != -1) {
            sim_len = atoi(argv[uarg]);
            if (sim_len <= 0) sim_len = -1;
        }
        assert(strlen(argv[targ]) < 500);
        strcpy(tfile, argv[targ]);
    }

// load the trace into memory
    printf("********************  CBP3  Start ***********************\n");
    printf("Trace:            %s\n", tfile);
    if (sim_len == -1)
        printf("Uops to simulate: Whole Trace\n\n");
    else
        printf("Uops to simulate: %i\n\n", sim_len);

    // check whether trace is compressed by bzip2
    bool bzip2 = false;
    if (strlen(tfile) > 3)
        bzip2 = (tfile[strlen(tfile) - 1] == '2' && 
                tfile[strlen(tfile) - 2] == 'z' && 
                tfile[strlen(tfile) - 3] == 'b');
    if (bzip2) {
        printf("Trace is compressed by bzip2.\n");
    }

    char cmd[600];
    sprintf (cmd, "%s %s", bzip2 ? "bzip2 -dc" : "cat", tfile);
	cout<< "parsing " <<  string(tfile) << endl;
	PG.parse_trace( string(tfile) );
	

// initialize predictors
    PredictorInit();
    
// keep running if rewind_marked is set
// the length of each run is decided by sim_len
    while (num_run == 0 ) {
        time_t run_start = time(NULL);
        run(sim_len);
        printf("Simulation Time: %ld Seconds\n", (time(NULL) - run_start));

		PG.reset();
    }

// the end...
    printf("\n********************  CBP3  End   ***********************\n");
    printf("Total Simulation Time: %ld Seconds\n", (time(NULL) - time_start));
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


// stats
uint64_t total_cycle;
uint64_t num_insts;
uint64_t num_uops;
uint64_t num_cond_br;
uint64_t num_ind_br;
uint64_t penalty_cond_br;    // misprediction penalty cycles of conditional branches
uint64_t penalty_ind_br;     // misprediction penalty cycles of indirect branches
uint64_t msp_cond_br;        // number of mispredictions of conditional branches
uint64_t msp_ind_br;         // number of mispredictions of indirect branches

void reset() {

    rewind_marked = false;
    cycle = 0;

    total_cycle = 0;
    num_insts = 0;
    num_uops = 0;
    num_cond_br = 0;
    num_ind_br = 0;
    penalty_cond_br = 0;
    penalty_ind_br = 0;
    msp_cond_br = 0;
    msp_ind_br = 0;

    PredictorReset();
}


void run(int sim_len) {
    num_run ++;
    printf("\n*********  RUN  %d   *********\n", (int) num_run);
    reset();
	PG.reset();
    uint16_t num_stages[NUM_STAGES];
    
    while (PG.Run_a_Cycle(num_stages) && // trace does not end
            (sim_len == -1 ? 1 : num_uops < (uint64_t)sim_len)) { // in the simulation range


        cycle = PG.get_cycle();
        cycle_info.cycle = cycle;
        cycle_info.num_fetch = num_stages[0];
        cycle_info.num_allocate = num_stages[1];
        cycle_info.num_exe = num_stages[2];
        cycle_info.num_retire = num_stages[3];
        cycle_info.num_agu = num_stages[4];
        cycle_info.num_std = num_stages[5];

        // fill in different fields into live_uop array for each stage

		PG.get_retire_uops( retire_uops );
		PG.get_fetch_uops( fetch_uops );
		PG.get_exe_uops( exe_uops );
		
		//on retire update stats
        for (uint64_t i = 0; i < cycle_info.num_retire; i ++) {

            cbp3_queue_entry_t *uop = retire_uops[i];
            uint16_t type = uop->uop.type;

            
            num_uops ++;
			num_insts ++; 
			
            //if (type & IS_EOM)            num_insts ++; // the uop is end of an inst
            if (type & IS_BR_CONDITIONAL) num_cond_br ++;
            if (type & IS_BR_INDIRECT) num_ind_br ++;


        }

		//on exe update mispredict info

        for (uint64_t i = 0; i < cycle_info.num_exe; i ++) {
            cbp3_queue_entry_t *uop = exe_uops[i];

            assert(uop->valid);

            // calculate mispred info
            uint16_t type = uop->uop.type;
            if (type & IS_BR_CONDITIONAL) {
                bool msp = (uop->uop.br_taken != (uop->last_pred > 0));
                if (!uop->cycle_last_pred) { // no prediction provided
                    msp = true;
                    penalty_cond_br += (cycle - uop->cycle_fetch);
                }else {
                    assert(uop->cycle_last_pred < cycle);
                    penalty_cond_br += ((msp ? cycle : uop->cycle_last_pred) - uop->cycle_fetch);
                }
                msp_cond_br += msp;
				//printf("BR executed: pred %u, actual %u\n", uop->last_pred, uop->uop.br_taken);
            }
            if (type & IS_BR_INDIRECT) {
                bool msp = (uop->uop.br_target != uop->last_pred);
                if (!uop->cycle_last_pred) {
                    msp = true;
                    penalty_ind_br += (cycle - uop->cycle_fetch);
                }else {
                    assert(uop->cycle_last_pred < cycle);
                    penalty_ind_br += ((msp ? cycle : uop->cycle_last_pred) - uop->cycle_fetch);
                }
                msp_ind_br += msp;
            }
            uop->cycle_exe = cycle;
        }

        PredictorRunACycle();
        total_cycle++;
    };

	

    // print out stats of a run
    printf("Total cycles:                         %llu\n\n", (long long unsigned) total_cycle);
    //printf("Num_Inst:                             %d\n", num_insts);
    //printf("Num_Uops:                             %d\n\n", num_uops);

    printf("Num_cond_br:                          %llu\n", (long long unsigned) num_cond_br);
    printf("Mispred_cond_br:                      %llu\n", (long long unsigned) msp_cond_br);
    printf("Mispred_penalty_cond_br:              %llu\n", (long long unsigned) penalty_cond_br);
    //printf("Conditional_MPKI:                     %.4f\n", (double)msp_cond_br/(double)num_insts*1000);
    printf("Conditional_MR:                       %.4f\n", (double)msp_cond_br/(double)num_cond_br);
    //printf("Final Score Run%i_Conditional_MPPKI:   %.4f\n\n", num_run, (double)penalty_cond_br/(double)num_insts*1000);
	printf("\n");

    printf("Num_ind_br:                           %llu\n", (long long unsigned) num_ind_br);
    printf("Mispred_ind_br:                       %llu\n", (long long unsigned) msp_ind_br);
    printf("Mispred_penalty_ind_br:               %llu\n", (long long unsigned) penalty_ind_br);
    //printf("Indirect_MPKI:                        %.4f\n", (double)msp_ind_br/(double)num_insts*1000);
    printf("Indirect_MR:                          %.4f\n", (double)msp_ind_br/(double)num_ind_br);
    //printf("Final Score Run%i_Indirect_MPPKI:      %.4f\n\n", num_run, (double)penalty_ind_br/(double)num_insts*1000);

    PredictorRunEnd();
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




bool  report_pred( cbp3_queue_entry_t* uop, uint64_t pred) {

    if (!uop->valid)
        return false;

	/*
    if (uop->cycle_exe) // uop has been executed
        return false;
		*/
    
    // record the prediction
    uop->cycle_last_pred = cycle;
    uop->last_pred = pred;
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
    last_pred = 0;
}


