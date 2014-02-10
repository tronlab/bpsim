/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#ifndef CONDITIONAL_H
#define CONDITIONAL_H

#include <cstdio>

#include "pred_table.h"
#include "pc_queue.h"

struct Predictions{
	int64_t meta;
	int64_t bimode;
	int64_t gshare0;
	int64_t gshare1;
	int64_t vote;
	void print(){
		printf( "meta:%d, bimode:%d, g0:%d, g1:%d, vote:%d\n", 
			(int)meta, (int)bimode, (int)gshare0, 
			(int)gshare1, (int)vote);
	}
};

class Conditional{
	Pred_Table* meta; 
	Pred_Table* bimode; 
	Pred_Table* gshare0;
	Pred_Table* gshare1;

	PC_Queue<Predictions> preds_q;

  public:
	Conditional( uint64_t hist_length0=10, uint64_t hist_length1=10 );
	~Conditional();
	int64_t predict( uint64_t pc );
	void update( uint64_t pc, bool taken);
	void reset();
	void print_stats();
	void finish(){ print_stats(); }

};

#endif /* ifndef CONDITIONAL_H */

