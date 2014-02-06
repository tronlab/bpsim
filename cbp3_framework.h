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
// Description: framework header for cbp3.
// DO NOT MODIFY. THIS FILE WILL NOT BE SUBMITTED WITH YOUR PREDICTOR.

#ifndef CBP3_FRAMEWORK_H_INCLUDED
#define CBP3_FRAMEWORK_H_INCLUDED

#define FETCHQ_SIZE 128
#define ROB_SIZE 256

// interface functions with the predictor

void PredictorInit();        // initialize predictors (e.g., allocating arrays)
void PredictorReset();       // reset predictor state at the beginning of a run
void PredictorRunACycle();   // called once at the end of each cycle to calculate predictions
void PredictorRunEnd();      // called at the end of a run
void PredictorExit();        // called at the end of simulation

class cbp3_queue_entry_t;
struct cbp3_cycle_activity_t;

// if the marker is set, the framework will rewind to the beginning of the trace
extern bool rewind_marked;

const cbp3_cycle_activity_t *get_cycle_info();            // what happened in the cycle
const cbp3_queue_entry_t    *fetch_entry(uint8_t e);      // access fetch queue
const cbp3_queue_entry_t    *rob_entry(uint8_t e);        // access rob
const int                    rename(uint8_t reg);         // access rename table
const int                    rename_flags();              // access rename table (for flags)
const uint64_t               reg_val(uint8_t reg);        // access architecture reg file

bool  report_pred( cbp3_queue_entry_t* entry, uint64_t pred );


cbp3_queue_entry_t* get_fetch_entry(uint8_t e);
cbp3_queue_entry_t* get_exe_entry(uint8_t e);
cbp3_queue_entry_t* get_retire_entry(uint8_t e);

struct cbp3_cycle_activity_t {
    // number of uops processed at each stage
    uint8_t  num_fetch;
    uint8_t  num_allocate;
    uint8_t  num_exe;
    uint8_t  num_retire;
    uint8_t  num_agu;
    uint8_t  num_std;


    // current cycle number (due to trace warmup, it does not start from 0)
    uint64_t cycle;
};

#endif
