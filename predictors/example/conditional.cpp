/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#include "conditional.h"

using namespace std;


/*Options:
 *	hist_length
 * 	table_size
 * 	pc_bits
 * 	counter_bits
 * 	indirect
 * 	is_tagged
 * 	historesis
 * 	reverse_hash
 * 	interleave
**/
Conditional::Conditional( uint64_t hist_length0, 
		uint64_t hist_length1 ){
	Options opts;

	opts.set( "assoc_bits",		0 );
	opts.set( "index_bits",		8 ); 
	opts.set( "pc_bits",		8 ); 
	opts.set( "hist_length",	0 ); 
	opts.set( "counter_bits",	2 );

	meta	= new Pred_Table( opts );
	bimode	= new Pred_Table( opts );

	opts.set( "index_bits",		12 ); 
	opts.set( "hist_length",	hist_length0 );
	gshare0	= new Pred_Table( opts );
	opts.set( "hist_length",	hist_length1 );
	opts.set( "reverse_pc",		1 );
	gshare1	= new Pred_Table( opts );
}

Conditional::~Conditional(){
	delete meta;
	delete bimode;
	delete gshare0;
	delete gshare1;
}

int64_t Conditional::predict( uint64_t pc ){

	Predictions preds;
	preds.meta			= meta->predict( pc );
	preds.bimode		= bimode->predict( pc );
	preds.gshare0		= gshare0->predict( pc );
	preds.gshare1		= gshare1->predict( pc );

	
	preds.vote = 0;
	if( preds.bimode >= 0 )
		preds.vote++;
	else
		preds.vote--;

	if( preds.gshare0 >= 0 )
		preds.vote++;
	else
		preds.vote--;

	if( preds.gshare1 >= 0 )
		preds.vote++;
	else
		preds.vote--;

	preds_q.push( pc, preds );

	if( preds.meta >= 0 ){
		return preds.bimode;
	}else{
		return preds.vote;
	}
}

void Conditional::update( uint64_t pc, bool taken ){
	
	preds_q.print_size();

	Predictions preds = preds_q.pop( pc );
	
	bool bimode_correct = ( (preds.bimode>=0) == taken );
	bool vote_correct = ( (preds.vote>=0) == taken );

	if( bimode_correct != vote_correct ){
		meta->update( pc, bimode_correct );
	}else{
		meta->update( pc );
	}

	bimode->update( pc, taken );

	if( bimode_correct ){
		gshare0->update( pc );
		gshare1->update( pc );
	}else{
		gshare0->update( pc, taken );
		gshare1->update( pc, taken );
	}
	
}

void Conditional::reset(){
	meta->reset();
	bimode->reset();
	gshare0->reset();
}

void Conditional::print_stats(){
	meta->print_stats();
	bimode->print_stats();
	gshare0->print_stats();
}




