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
#include <iostream>

#include "cbp3_reader.h"

using namespace std;


void print_cycle_info( const cbp3_cycle_info_t *info, const uint16_t *num_stage ){

    cout << "\n@@@NUM_STAGE\n";
    cout << "@@@num_fetch:\t\t"	    << num_stage[0] << "\n";
    cout << "@@@num_allocate:\t"	<< num_stage[1] << "\n";	
    cout << "@@@num_retire:\t\t"	<< num_stage[3] << "\n";
    cout << "@@@num_agu:\t\t"       << num_stage[4] << "\n";
    cout << "@@@num_std:\t\t"       << num_stage[5] << "\n";

    //q's
	cout << "\n@@@CYCLE INFO: " << info->cycle << "\n";

	cout << "@@@Q's\n";
	cout << "@@@fetch\n";
    for(int i=0; i<num_stage[0]; i++)
        //cout << "@@@" << info->fetch_q[i] << "\n";
        cout << "@@@" << (int) info->fetch_q[i] << "\n";

	cout << "@@@allocate\n";
    for(int i=0; i<num_stage[1]; i++)
        //cout << "@@@" << info->allocate_q[i] << "\n";
        cout << "@@@" << (int) info->allocate_q[i] << "\n";

	cout << "@@@exe\n";
    for(int i=0; i<num_stage[2]; i++)
        //cout << "@@@" << info->exe_q[i] << "\n";
        cout << "@@@" << (int) info->exe_q[i] << "\n";
    
	cout << "@@@retire\n";
    for(int i=0; i<num_stage[3]; i++)
        //cout << "@@@" << info->retire_q[i] << "\n";
        cout << "@@@" << (int) info->retire_q[i] << "\n";

	cout << "@@@agu\n";
    for(int i=0; i<num_stage[4]; i++)
        //cout << "@@@" << info->agu_q[i] << "\n";
        cout << "@@@" << (int) info->agu_q[i] << "\n";

	cout << "@@@std\n";
    for(int i=0; i<num_stage[5]; i++)
        //cout << "@@@" << info->std_q[i] << "\n";
        cout << "@@@" << (int) info->std_q[i] << "\n";
	//cout << "\n";

	for(int i=0; i<num_stage[0]; i++){
		const cbp3_uop_dynamic_t& uop = info->uopinfo[i];

		cout << "@@@ uop: " << i << "\n";
		cout << "@@@ PC: " << uop.pc << "\n";
		//print type

		if( uop.type & IS_BR_CONDITIONAL )
			cout << "@@@ IS_BR_CONDITIONAL\n";
		if( uop.type & IS_BR_INDIRECT )
			cout << "@@@ IS_BR_INDIRECT\n";
		if( uop.type & IS_BR_CALL )
			cout << "@@@ IS_BR_CALL\n";
		if( uop.type &  IS_BR_RETURN)
			cout << "@@@ IS_BR_RETURN\n";
		if( uop.type &  IS_BR_OTHER)
			cout << "@@@ IS_BR_OTHER\n";
		if( uop.type &  IS_LOAD)
			cout << "@@@ IS_LOAD\n";
		if( uop.type &  IS_STORE)
			cout << "@@@ IS_STORE\n";
		if( uop.type &  IS_FP)
			cout << "@@@ IS_FP\n";
		if( uop.type &  IS_WFLAGS)
			cout << "@@@ IS_WFLAGS\n";
		if( uop.type &  IS_RFLAGS)
			cout << "@@@ IS_RFLAGS\n";
		if( uop.type &  IS_BOM)
			cout << "@@@ IS_BOM\n";
		if( uop.type &  IS_EOM)
			cout << "@@@ IS_EOM\n";

	}





       /* 

        uint32_t br_target;                 // real branch target
        bool     br_taken;                  // real branch direction

        // static info

        // uop_id is an unique id used in trace reader
        // it is reset to 0 in the framework and not supposed to be used by predictors
        uint32_t uop_id;

        uint8_t  inst_size;                 // size of an instruction (in bytes)
        uint32_t pc;                        // address of an instruction
        uint16_t type;                      // uop type ( refer to IS_* encodings defined above)
        uint8_t  opsize;                    // op size (in bytes)
        uint8_t  srcs_static[UOP_SRCS_MAX]; // static src registers
        uint8_t  dst_static;                // static dst register
        */
}
