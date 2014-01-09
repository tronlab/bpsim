#ifndef __PROGRAM_GRAPH_H__
#define __PROGRAM_GRAPH_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <deque>
#include <inttypes.h>
#include <assert.h>

#include "cbp3_def.h"


#define WHERESTR  "[file %s, line %d]: "
#define WHEREARG  __FILE__, __LINE__
#define DEBUGPRINT2(...)       fprintf(stderr, __VA_ARGS__)
#define DEBUGPRINT(_fmt, ...)  DEBUGPRINT2(WHERESTR _fmt, WHEREARG, __VA_ARGS__)

#define RETIRE_DELAY 7
#define EXE_DELAY 5
//#define PIPE_WIDTH 4


class Program_Graph;
struct Branch;
class UOP_Queue;



class UOP_Queue{
	//std::deque<std::vector<queue_elt> > queue;
	std::deque<std::vector<cbp3_queue_entry_t>* > queue;
	uint64_t cycle;
  public:
	void reset();
	void push( const Branch &branch );
	void next_cycle();
	void populate_fetch_uop( std::vector<cbp3_queue_entry_t*>& fetch_uops );
	void populate_exe_uop( std::vector<cbp3_queue_entry_t*>& exe_uops );
	void populate_retire_uop( std::vector<cbp3_queue_entry_t*>& retire_uops );
	void populate_num_uops( uint16_t* num_stages );
	uint64_t get_cycle();
	void print();
	
};

struct Branch{
	uint16_t 	type;
	uint64_t 	pc;
	uint64_t 	target;
	bool		taken;
	void print(){
		std::cout << type << " " << pc << " " << target << " " << taken << "\n";
	}
};


class Program_Graph{

	UOP_Queue uop_queue;
	std::vector<Branch> branches;
	uint64_t current_branch;


	

  public:
	void parse_trace( std::string );
	void print_trace();
	
	void reset();
	bool Run_a_Cycle( uint16_t* num_stages );

	uint64_t get_cycle();
	void get_retire_uops( std::vector<cbp3_queue_entry_t*> &retire );
	void get_exe_uops( std::vector<cbp3_queue_entry_t*> &exe );
	void get_fetch_uops( std::vector<cbp3_queue_entry_t*> &fetch );
};


#endif

