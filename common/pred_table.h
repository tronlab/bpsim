/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#ifndef PRED_TABLE_H
#define PRED_TABLE_H
#include <vector>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <stdint.h>

#include "opts.h"
#include "hash.h"
#include "pc_queue.h"

#define NUM_CONST_ADDR_BITS 2




class History{
	Options &opts;
	std::deque<uint64_t> target_hist;
	std::deque<bool> taken_hist;
  public:
	History( Options& );
	uint64_t operator[]( size_t n );
	size_t size();
	void reverse();
	void update( uint64_t );
	void update( bool );

	void print();
	void print_target_hist();
	void print_taken_hist();

	//TODO change this to returning a uint64_t
	const std::deque<uint64_t> &get_target_hist(){ return target_hist; }
	const std::deque<bool> &get_taken_hist(){ return taken_hist; }
};

class Table_Entry{
	Options &opts;
	
	uint64_t 	tag;	//using the pc
	int64_t 	target;
	bool		resist; //used if historesis is set

  public:

	Table_Entry( Options& );
	bool check_tag( uint64_t );
	int64_t get_contents();
	void update( uint64_t, int64_t );
	void update( uint64_t, bool );
	void set( uint64_t, int64_t );
	void print();
};

class Ways{
	Options &opts;
	std::vector<Table_Entry> ways;
  public:
	Ways( Options& );
	Table_Entry &get_entry( uint64_t pc );//index includes tag
	//size_t size(){ return ways.size(); }
	bool has_pc( uint64_t pc );
	void allocate( uint64_t pc, int64_t target );
	void print();
};

class Sets{
	Options &opts;
	std::vector<Ways> sets;
  public:
	Sets( Options& );
	Table_Entry &get_entry(  uint64_t pc, uint64_t index );//index includes tag
	bool has_pc( uint64_t pc, uint64_t index );
	//size_t size(){ return sets.size(); }
	void allocate( uint64_t key, int64_t target );
	void print();
};

class Pred_Table{
	Options	opts;
	History hist;
	Sets	sets;
	Hash	hash;

	PC_Queue<uint64_t> last_index;
	uint64_t get_index( uint64_t pc );

  public:

	Pred_Table( Options opts );
	~Pred_Table(){}

	bool has_pc( uint64_t pc );
	int64_t predict( uint64_t pc );
	//TODO: templatize update?
	void update( uint64_t pc, uint64_t target );
	void update( uint64_t pc, bool taken );
	void update( uint64_t pc );

	void set( uint64_t pc, int64_t val );


	void reset() {}
	void print_stats() {}

};


#endif /* ifndef PRED_TABLE_H */

