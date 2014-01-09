#include "program_graph.h"

#define UN_INIT_LAST_PC (0xffffffff)

using namespace std;

static bool is_legal( uint16_t instr){
	uint16_t mask = 
		IS_BR_CONDITIONAL	|
		IS_BR_INDIRECT		;

	return (mask & instr) || !(instr);
}

static bool is_branch( uint16_t instr){
	uint16_t mask = 
		IS_BR_CONDITIONAL	|
		IS_BR_INDIRECT		|
		IS_BR_CALL			|
		IS_BR_RETURN		|
		IS_BR_OTHER;
	return (mask & instr);
}

static inline bool is_cond( uint8_t instr ){
	return (instr & IS_BR_CONDITIONAL);
}

/*
 * Program Graph
 */

void Program_Graph::parse_trace( string filename ){
/*
 * format:
 * 		(other)		instr
 * 		(branch)	instr pc target taken?
 */
	uint16_t	instr;

	ifstream trace;

	trace.open( filename.c_str() );

	trace >> instr;
	while( trace.good() ){

		assert( is_legal(instr) );

		//if branch
		if( is_branch(instr) ){

			Branch branch;

			//parse line
			branch.type	= instr;
			trace >> branch.pc;
			trace >> branch.target;
			trace >> branch.taken;

			branches.push_back( branch );
		}		

		trace >> instr;
	}
	//cout<< "here" <<endl;
	//print_trace();
}



void Program_Graph::print_trace(){
	reset();
	for( unsigned i=0; i<branches.size(); i++){
		branches[i].print();
	}
	reset();
}

void Program_Graph::reset(){
	current_branch = 0;
	uop_queue.reset();
}

bool Program_Graph::Run_a_Cycle( uint16_t* num_stages ){
	
	//check if there is another node to run
	if( current_branch != branches.size() ){

		uop_queue.populate_num_uops( num_stages );
		uop_queue.next_cycle();

		uop_queue.push( branches[current_branch] );

		current_branch++;


		return true;
	}else{
		return false;
	}
}


uint64_t Program_Graph::get_cycle(){
	return uop_queue.get_cycle();
}

void Program_Graph::get_retire_uops( vector<cbp3_queue_entry_t*> &retire_uops ){
	uop_queue.populate_retire_uop( retire_uops );
}

void Program_Graph::get_fetch_uops( vector<cbp3_queue_entry_t*> &fetch_uops ){
	uop_queue.populate_fetch_uop( fetch_uops );
}

void Program_Graph::get_exe_uops( vector<cbp3_queue_entry_t*> &exe_uops ){
	uop_queue.populate_exe_uop( exe_uops );
}


/*
 * uop_queue
 */


//delay currently unused
void UOP_Queue::push( const Branch &branch ){

	vector<cbp3_queue_entry_t>* front = queue.front();

	cbp3_queue_entry_t uop;

	uint64_t target = branch.target;
	bool taken = branch.taken;


	uop.valid 	= true;

	// fill fields that are available at fetch stage
	uop.uop.pc = branch.pc;
	uop.uop.type = branch.type;
	
	uop.cycle_fetch = cycle;

	// yes, br_target and direction is available for br history update
	uop.uop.br_target = target;
	uop.uop.br_taken = taken;

	
	front->push_back( uop );
}

void UOP_Queue::next_cycle(){
	cycle++;
	vector<cbp3_queue_entry_t>* uop_vec = new vector<cbp3_queue_entry_t>();
	queue.push_front( uop_vec );

	if( queue.size() > RETIRE_DELAY + 1){
		uop_vec = queue.back();
		queue.pop_back();
		delete uop_vec;
	}

	//update the cycles when things occured
	vector<cbp3_queue_entry_t>& exe_list = *queue.at( EXE_DELAY );
	for(unsigned i=0; i<exe_list.size(); i++){
		exe_list[i].cycle_exe = cycle;
	}

	vector<cbp3_queue_entry_t>&retire_list = *queue.at( RETIRE_DELAY );
	for(unsigned i=0; i<retire_list.size(); i++){
		retire_list[i].cycle_retire = cycle;
	}
}

void UOP_Queue::populate_fetch_uop( vector<cbp3_queue_entry_t*> &fetch ){
	vector<cbp3_queue_entry_t>& fet_vec = *queue.front();


	fetch.resize( fet_vec.size() );
	for(unsigned i=0; i<fet_vec.size(); i++){
		fetch[i] = &fet_vec[i];
	}
}

void UOP_Queue::populate_exe_uop( vector<cbp3_queue_entry_t*> &exe ){
	vector<cbp3_queue_entry_t>& exe_vec = *queue.at( EXE_DELAY);

	exe.resize( exe_vec.size() );
	for(unsigned i=0; i<exe_vec.size(); i++){
		exe[i] = &exe_vec[i];
	}
}

void UOP_Queue::populate_retire_uop( vector<cbp3_queue_entry_t*> &retire ){
	vector<cbp3_queue_entry_t>& ret_vec = *queue.back();

	retire.resize( ret_vec.size() );
	for(unsigned i=0; i<ret_vec.size(); i++){
		retire[i] = &ret_vec[i];
	}
}

void UOP_Queue::populate_num_uops( uint16_t* num_stages ){

        num_stages[0] = queue.front()->size();
        num_stages[1] = 0;
        num_stages[2] = queue.at(EXE_DELAY)->size();
        num_stages[3] = queue.back()->size();
        num_stages[4] = 0;
        num_stages[5] = 0;

}

uint64_t UOP_Queue::get_cycle(){
	return cycle;
}

void UOP_Queue::reset(){
	cycle = 0;

	queue.resize( RETIRE_DELAY+1 );

	for( unsigned i=0; i<queue.size(); i++){
		if( queue[i] )
			queue[i]->clear();
		else
			queue[i] = new vector<cbp3_queue_entry_t>();
	}
}

static void print_uop( const cbp3_queue_entry_t& uop ){
	cout 
		<< uop.uop.type << " " 
		<< uop.uop.pc << " " 
		<< uop.uop.br_target << " " 
		<< uop.uop.br_taken << endl;

}

static void print_queue_vec( const vector<cbp3_queue_entry_t>& vec ){
	for(unsigned i=0; i<vec.size(); i++){
		print_uop( vec[i] );
	}
}

void UOP_Queue::print(){
	vector<cbp3_queue_entry_t>& fet_vec = *queue.front();

	print_queue_vec( fet_vec );

}








