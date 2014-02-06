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
