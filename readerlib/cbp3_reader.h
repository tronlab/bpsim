#ifndef CBP3_READER_H_INCLUDED
#define CBP3_READER_H_INCLUDED

#include <stdint.h>
#include <cbp3_def.h>

// ************** The following code is used to interface trace reader **************
// **************        DO NOT use them in your predictor code        **************

// cycle activity and uops read from reader
class cbp3_cycle_info_t {
    public:
        cbp3_cycle_info_t (){ reset(); }
        void reset();

        cbp3_uop_dynamic_t uopinfo[PIPE_WIDTH];
        uint8_t  fetch_q[PIPE_WIDTH], allocate_q[PIPE_WIDTH], exe_q[15];
        uint8_t  retire_q[PIPE_WIDTH], agu_q[2], std_q[8];
        uint32_t cycle;
};

// reader interface

void ReaderLoadTrace(FILE *cbp3_raw_trace_fp, bool load, bool debug, int value_set, int mem_set);
//loads the trace file

bool ReaderRunACycle(uint16_t *num_stage);
/*
   Populates the following fields
        cycle_info.num_fetch = num_stages[0];
        cycle_info.num_allocate = num_stages[1];
        cycle_info.num_exe = num_stages[2];
        cycle_info.num_retire = num_stages[3];
        cycle_info.num_agu = num_stages[4];
        cycle_info.num_std = num_stages[5];
*/

const cbp3_cycle_info_t * ReaderInfo();
/*
   Populates the following fields
    
*/
void print_cycle_info( const cbp3_cycle_info_t *cycle_info, const uint16_t *num_stage );





void ReaderRewind();
//resets the simulator



//who cares?

//return true
bool ReaderTraceCheck();
//the size of the trace -return 0

uint32_t ReaderMaxMem();
//the size of the trace -return 0

uint32_t ReaderTraceInfoSize();
//never called

void ReaderEnd();

#endif
