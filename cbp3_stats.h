/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see LICENSE for license information.
 */

#ifndef CBP3_STATS_H
#define CBP3_STATS_H

#include <vector>
#include <iostream>
#include <cstdio>
#include <stdint.h>

class cbp3_stats {
// stats

	uint64_t num_uops;

	uint64_t num_taken;
	uint64_t num_not_taken;        

	uint64_t num_ind_br;
	uint64_t msp_ind_br;        

	uint64_t num_cond_br;
	uint64_t msp_cond_br;        

	uint64_t num_both;
	uint64_t msp_both[8];
  public:
	uint64_t total_cycle;


	cbp3_stats(){ reset(); }
	//~cbp3_stats();
	void reset();
	void print();
	void update( bool, bool, bool, bool, bool );
	uint64_t get_num_uops(){ return num_uops; }

};


#endif /* ifndef CBP3_STATS_H */

