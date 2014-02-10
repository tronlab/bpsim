/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#include <stdio.h>
#include <cassert>
#include <string>
#include <sstream>
#include <inttypes.h>

#include "cbp3_def.h"
#include "cbp3_framework.h"
#include "predictor.h"
#include "conditional.h"
#include "indirect.h"

static Conditional* conditional;
static Indirect* indirect;

void PredictorInit() {
	conditional	= new Conditional();
	indirect	= new Indirect();
}

void PredictorInit( int hist_length ) {
	conditional	= new Conditional( hist_length );
	indirect	= new Indirect();
}

void PredictorInit( int hist_length0, int hist_length1 ) {
	conditional	= new Conditional( hist_length0, hist_length1 );
	indirect	= new Indirect();
}

void PredictorReset() {
    // this function is called before EVERY run
    // it is used to reset predictors and change configurations
	conditional->reset();
	indirect->reset();
}

void PredictorRunACycle() {
    // get info about what uops are processed at each pipeline stage
    const cbp3_cycle_activity_t *cycle_info = get_cycle_info();

    // make prediction at fetch stage
    for (int i = 0; i < cycle_info->num_fetch; i++) {
        cbp3_queue_entry_t *entry 	= get_fetch_entry( i );
		cbp3_uop_dynamic_t *uop		= &entry->uop;

        if ( uop->type & IS_BR_CONDITIONAL ){
            int64_t pred = conditional->predict( uop->pc );
            assert(report_taken_pred(entry, pred));
        }

        if ( uop->type & IS_BR_INDIRECT ){
            uint64_t pred = indirect->predict( uop->pc );
            assert(report_target_pred(entry, pred));
        }
    }

	// update at the retire stage
    for (int i = 0; i < cycle_info->num_retire; i++) {

        const cbp3_queue_entry_t *entry = get_retire_entry(i);
        const cbp3_uop_dynamic_t *uop = &entry->uop;

        if( uop->type & IS_BR_CONDITIONAL ){
            bool t = uop->br_taken;
			conditional->update( uop->pc, t );
        }

        if ( uop->type & IS_BR_INDIRECT ){
			//if a branch is not taken, the target is not valid
			if( uop->br_taken ){
				uint64_t target = uop->br_target;
				indirect->update( uop->pc, target );
			}
        }

    }
}

void PredictorRunEnd() {
	conditional->finish();
	indirect->finish();
}

void PredictorExit() {
	delete conditional;
	delete indirect;
}

