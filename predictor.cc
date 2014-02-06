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
// Description: sample predictors for cbp3.

#include <stdio.h>
#include <cassert>
#include <string.h>
#include <inttypes.h>

using namespace std;
// #include "cbp3_def.h"
#include "cbp3_framework.h"

// this file includes two sample predictors
// one is a 64 KB gshare conditional predictor
// the other is a 64 KB indirect predictor indexed by (pc ^ history)

// rewind_marked is also used to show how to rewind the reader for multiple runs
// the predictor will use gshare in the first run
// the indirect predictor will be used in the second run

// NOTE: rewind_marked is only provided to help tuning work. the final
// submitted code should only include one run.


#define GSHARE_SIZE 18 // 256K 2-bit counters = 64 KB cost
#define IND_SIZE 14    // 16K 32-bit targets  = 64 KB cost

// predictor tables
int8_t   *gtable;
uint64_t *indtable;

// two branch history registers:
// the framework provids real branch results at fetch stage to simplify branch history
// update for predicting later branches. however, they are not available until execution stage
// in a real machine. therefore, you can only use them to update predictors at or after the
// branch is executed.
// in this sample code, we update predictors at retire stage where uops are processed
// in order to enable easy regneration of branch history.

// cost: depending on predictor size
uint64_t brh_fetch;
uint64_t brh_retire;

// count number of runs
uint64_t runs;

void PredictorInit() {
    runs = 0;
    gtable = new int8_t[1 << GSHARE_SIZE];
    indtable = new uint64_t[1 << IND_SIZE];
    assert(gtable && indtable);
}

void PredictorReset() {
    // this function is called before EVERY run
    // it is used to reset predictors and change configurations

    if (runs == 0)
        printf("Predictor:gshare\nconfig: %i counters, %i KB cost\n", 1 << GSHARE_SIZE, (1 << GSHARE_SIZE) * 2 / 8 / 1024);
    else
        printf("Predictor:ind\nconfig: %i targets,  %i KB cost\n", 1 << IND_SIZE, (1 << IND_SIZE) * 4 / 1024);

    for (int i = 0; i < (1 << GSHARE_SIZE); i ++)
        gtable[i] = 0;
    for (int i = 0; i < (1 << IND_SIZE); i ++)
        indtable[i] = 0;

    brh_fetch = 0;
    brh_retire = 0;
}

void PredictorRunACycle() {
    // get info about what uops are processed at each pipeline stage
    const cbp3_cycle_activity_t *cycle_info = get_cycle_info();

    // make prediction at fetch stage
	//printf("%u instructions fectched\n", cycle_info->num_fetch);
    for (int i = 0; i < cycle_info->num_fetch; i++) {
        cbp3_queue_entry_t *entry 	= get_fetch_entry( i );
		cbp3_uop_dynamic_t *uop		= &entry->uop;
		//printf( "uop: %u\n", uop);
		//uint16_t type = uop->valid;

		//assert( uop->type );

        if (runs == 0 && (uop->type & IS_BR_CONDITIONAL) ){
            // get prediction
            uint64_t gidx = (brh_fetch ^ uop->pc) & ((1 << GSHARE_SIZE) - 1);
            bool gpred = (gtable[gidx] >= 0);

            // report prediction:
            // you need to provide direction predictions for conditional branches,
            // targets of conditional branches are available at fetch stage.
            // for indirect branches, you need to provide target predictions.
            // you can report multiple predictions for the same branch
            // the framework will use the last reported prediction to calculate 
            // misprediction penalty
            assert(report_pred(entry, gpred));
        }else if (runs == 1 && uop->type & IS_BR_INDIRECT) {
            uint64_t gidx = (brh_fetch ^ uop->pc) & ((1 << IND_SIZE) - 1);
            uint64_t gpred = indtable[gidx];

            assert(report_pred(entry, gpred));
        }

        // update fetch branch history
        if (uop->type & IS_BR_CONDITIONAL)
            brh_fetch = (brh_fetch << 1) | (uop->br_taken ? 1 : 0);
        else if (uop_is_branch(uop->type))
            brh_fetch = (brh_fetch << 1) | 1;
    }

    for (int i = 0; i < cycle_info->num_retire; i++) {

        const cbp3_queue_entry_t *entry = get_retire_entry(i);
        const cbp3_uop_dynamic_t *uop = &entry->uop;

        if (runs == 0 && uop->type & IS_BR_CONDITIONAL) {
            uint64_t gidx = (brh_retire ^ uop->pc) & ((1 << GSHARE_SIZE) - 1);

            // update predictor
            bool t = uop->br_taken;
            if (t && gtable[gidx] < 1)
                gtable[gidx] ++;
            else if (!t && gtable[gidx] > -2)
                gtable[gidx] --;
        }else if (runs == 1 && uop->type & IS_BR_INDIRECT) {
            uint64_t gidx = (brh_retire ^ uop->pc) & ((1 << IND_SIZE) - 1);
            indtable[gidx] = uop->br_target;
        }

        // update retire branch history
        if (uop->type & IS_BR_CONDITIONAL)
            brh_retire = (brh_retire << 1) | (uop->br_taken ? 1 : 0);
        else if (uop_is_branch(uop->type))
            brh_retire = (brh_retire << 1) | 1;
    }
}

void PredictorRunEnd() {
    runs ++;
    if (runs < 2) // set rewind_marked to indicate that we want more runs
        rewind_marked = true;
}

void PredictorExit() {
    delete [] gtable;
    delete [] indtable;
}
