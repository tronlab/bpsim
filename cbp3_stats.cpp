/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see LICENSE for license information.
 */

#include <iostream>
#include "cbp3_stats.h"


void cbp3_stats::update( bool taken, bool cond, bool ind, bool cond_msp, bool ind_msp ){
	
	num_uops++;


	if( cond && ind ){
		num_both++;

		unsigned idx = 0;
		if( taken )
			idx |= (1<<0);
		if( cond_msp )
			idx |= (1<<1);
		if( ind_msp )
			idx |= (1<<2);

		msp_both[idx]++;


		return;
	}

	if( cond ){
		num_cond_br++;
		if( cond_msp )
			msp_cond_br++;

		if( taken )
			num_taken++;
		else
			num_not_taken++;
		return;
	}

	if( ind ){
		num_ind_br++;
		if( ind_msp )
			msp_ind_br++;
		return;
	}

	//neither conditional nor indirect
}


void cbp3_stats::reset(){
    num_uops = 0;

    num_not_taken = 0;
    num_taken = 0;

    num_cond_br = 0;
    num_ind_br = 0;

    msp_cond_br = 0;
    msp_ind_br = 0;

    num_both = 0;
	for( int i=0; i<8; i++){
		msp_both[i] = 0;
	}
}


void cbp3_stats::print(){
    printf("**************** Stats Start *********************\n");

    printf("Total cycles:                         %llu\n", (long long unsigned) total_cycle);
    printf("Total uops:                           %llu\n\n", (long long unsigned) num_uops);

    printf("num_taken:                            %llu\n", (long long unsigned) num_taken);
    printf("num_not_taken:                        %llu\n\n", (long long unsigned) num_not_taken);

    printf("Num_cond_br:                          %llu\n", (long long unsigned) num_cond_br);
    printf("Mispred_cond_br:                      %llu\n", (long long unsigned) msp_cond_br);
    printf("Conditional_MR:                       %.4f\n", (double)msp_cond_br/(double)num_cond_br);
	printf("\n");

    printf("Num_ind_br:                           %llu\n", (long long unsigned) num_ind_br);
    printf("Mispred_ind_br:                       %llu\n", (long long unsigned) msp_ind_br);
    printf("Indirect_MR:                          %.4f\n\n", (double)msp_ind_br/(double)num_ind_br);

    printf("**************** Stats End ***********************\n");

}


