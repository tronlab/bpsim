/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#include "pred_table.h"

using namespace std;

/******************************************************************************/
/***************** History ****************************************************/
/******************************************************************************/

History::History( Options& _opts ):
	opts(_opts),
	target_hist( opts["hist_length"], 0 ),
	taken_hist( opts["hist_length"], 0 )
{
}

/*
void History::reverse(){
	std::reverse( target_hist.begin(), target_hist.end() );
}
*/

void History::update( uint64_t target ){

	//reverse means hist[0] is the oldest target
	if( opts["reverse_hist"] ){
		target_hist.pop_front();
		target_hist.push_back( target >> NUM_CONST_ADDR_BITS );
	}else{
		target_hist.pop_back();
		target_hist.push_front( target >> NUM_CONST_ADDR_BITS );
	}
	assert( target_hist.size() == opts["hist_length"] );
}

void History::update( bool taken ){
	taken_hist.pop_back();
	taken_hist.push_front( taken );
}

uint64_t History::operator[]( size_t n ){ 
	if( opts["indirect"] )
		return target_hist[n]; 
	else 
		return taken_hist[n];
}

uint64_t History::size(){ 
	if( opts["indirect"] )
		return target_hist.size();
	else 
		return taken_hist.size();
}

void History::print(){
	if( opts["indirect"] )
		print_target_hist();
	else 
		print_taken_hist();
}

void History::print_target_hist(){ 
	deque<uint64_t>::iterator it;
	for(	it = target_hist.begin();
			it != target_hist.end();
			++it
	   ){
		cout << *it << ":";
	}
	cout << endl;
}

void History::print_taken_hist(){ 
	deque<bool>::iterator it;
	for(	it = taken_hist.begin();
			it != taken_hist.end();
			++it
	   ){
		cout << *it << ":";
	}
	cout << endl;
}

/******************************************************************************/
/***************** Table_Entry ************************************************/
/******************************************************************************/

Table_Entry::Table_Entry( Options& _opts ): opts(_opts)
{
}

bool Table_Entry::check_tag( uint64_t pc ){
	if( opts["is_tagged"] )
		return (tag==pc);
	else
		return true;
}

int64_t Table_Entry::get_contents(){

	return target;
}

void Table_Entry::update( uint64_t pc, bool increment ){
	assert( !opts["indirect"] );

	if( ! check_tag(pc) ){
		target = 0;
	}

	if( increment )
		target++;
	else
		target--;

	int max = (1<<(opts["counter_bits"]-1))-1;
	int min = -(1<<(opts["counter_bits"]-1));


	if( target > max )
		target = max;

	if( target < min )
		target = min;

}

void Table_Entry::update( uint64_t pc, int64_t new_target ){
	assert( opts["indirect"] );

	//if entry is correct, reset resist and change nothing
	if( check_tag(pc) && (target==new_target) ){
		resist = opts["historesis"];
		return;
	}

	//allows entry to update only after 2 wrong targets and/or keys
	if( resist ){
		resist = false;
		return;
	}

	resist	= opts["historesis"];
	tag		= pc;
	target	= new_target;
}

void Table_Entry::set( uint64_t pc, int64_t val ){
	resist	= opts["historesis"];
	tag		= pc;
	target	= val;
}

void Table_Entry::print(){
	printf( "%llu\t%lld\t%d\n", (long long unsigned) tag, (long long ) target, resist );
}


/******************************************************************************/
/***************** Ways ********************************************************/
/******************************************************************************/

Ways::Ways( Options& _opts ):
	opts(_opts),
	ways( 1<<opts["assoc_bits"], Table_Entry(opts) )
{
}

Table_Entry &Ways::get_entry( uint64_t pc ){
	for( unsigned i=0; i<ways.size(); ++i ){
		if( ways[i].check_tag(pc) )
			return ways[i];
	}
	return ways[0];
}

bool Ways::has_pc( uint64_t pc ){

	for( unsigned i=0; i<ways.size(); ++i ){
		if( ways[i].check_tag( pc ) )
			return true;
	}
	return false;
}

void Ways::allocate( uint64_t pc, int64_t target ){
	//random replacement
	int idx = rand()%ways.size();
	ways[idx].set( pc, target );
} 

void Ways::print(){
	for( unsigned i=0; i<ways.size(); i++ ){
		printf( "%d\t", i );
		ways[i].print();
	}
	printf( "\n" );
}

/******************************************************************************/
/***************** Sets *******************************************************/
/******************************************************************************/

Sets::Sets( Options& _opts ): 
	opts(_opts),
	sets( 1<<opts["index_bits"], Ways(opts) )
{

	if( !opts["indirect"] )
		assert( opts["counter_bits"] );
}

bool Sets::has_pc( uint64_t pc, uint64_t index ){
	assert( index < sets.size() );
	return sets[index].has_pc(pc);
}

Table_Entry &Sets::get_entry( uint64_t pc, uint64_t index ){
	assert( index < sets.size() );
	return sets[index].get_entry( pc );
}

void Sets::allocate( uint64_t key, int64_t target ){
	return sets[key%sets.size()].allocate( key, target );
}

void Sets::print(){
	
	printf( "set\tidx\ttag\ttarget\tresist\n" );
	for( unsigned i=0; i<sets.size(); i++ ){
		printf( "%d\t", i );
		sets[i].print();
	}
}

/******************************************************************************/
/***************** Pred_Table *************************************************/
/******************************************************************************/

	
Pred_Table::Pred_Table( Options _opts ):
	opts( _opts),
	hist( opts ),
	sets( opts ),
	hash( opts )
{
	opts.check_invariants();
}

uint64_t Pred_Table::get_index( uint64_t pc ){
	if( opts["indirect"] )
		return hash( pc, hist.get_target_hist() );
	else
		return hash( pc, hist.get_taken_hist() );
}

bool Pred_Table::has_pc( uint64_t pc ){
	uint64_t index = get_index( pc );
	return sets.has_pc( pc, index );
}

int64_t Pred_Table::predict( uint64_t pc ){
	assert( hist.size() == opts["hist_length"] );

	uint64_t index = get_index( pc );

	last_index.push( pc, index );

	Table_Entry &entry = sets.get_entry( pc, index );


	if( !entry.check_tag(pc) ){
		return 0; //return default value
	}

	return entry.get_contents();
}

void Pred_Table::update( uint64_t pc, uint64_t target ){
	assert( opts["indirect"] );

	uint64_t index = last_index.pop( pc );

	if( sets.has_pc( pc, index ) ){
		Table_Entry &entry = sets.get_entry( pc, index );
		entry.update( pc, (int64_t)target );
	}else{
		sets.allocate( index, target );
	}

	
	/*
	hist.print_target_hist();
	printf( "update: key, index, pc, target\n" );
	printf( "%llu\t%llu\t%llu\t%llu\n", (long long unsigned)key, 
			(long long unsigned)key % (1<<opts["index_bits"]),
			(long long unsigned)pc, (long long unsigned)target );
	sets.print();
	std::cout << std::endl;
	*/


	if( opts["hist_length"] )
		hist.update( target );
}

void Pred_Table::update( uint64_t pc ){
	last_index.pop( pc );
}

void Pred_Table::update( uint64_t pc, bool taken ){

	assert( !opts["indirect"] );
	assert( hist.size() == opts["hist_length"] );

	uint64_t index = last_index.pop( pc );

	Table_Entry &entry = sets.get_entry( pc, index );

	entry.update( pc, taken );


	if( opts["hist_length"] )
		hist.update( taken );


}

void Pred_Table::set( uint64_t pc, int64_t val ){

	assert( !opts["indirect"] );

	uint64_t index = last_index.pop( pc );

	Table_Entry &entry = sets.get_entry( pc, index );

	entry.set( pc, val );

}




