/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 *
 * 2 Level Indirect Predictor
 * tagless
 * 256 Entry, 
 * 2 target history length,
 * reverse interleaving, 
 * 2 updates required
 */

#include "indirect.h"

using namespace std;

Indirect::Indirect(){
	Options opts;
	/*
	*/
	opts.set( "indirect",		1 );
	opts.set( "assoc_bits",		0 );
	opts.set( "index_bits",		6 ); // 64 entries
	opts.set( "pc_bits",		6 ); // same as number of entries
	opts.set( "hist_length",	0 );
	opts.set( "is_tagged",		0 ); //
	opts.set( "historesis",		1 ); // update after 2 hits
	opts.set( "reverse_hist",	1 ); // older hist is more important
	opts.set( "interleave",		1 ); // mix hist instead of concatinate

	//opts.print();
	stages.push_back( new Pred_Table( opts ) );

	opts.set( "hist_length",	1 );
	opts.set( "index_bits",		6 ); // 64 entries
	opts.set( "pc_bits",		6 ); // same as number of entries
	opts.set( "is_tagged",		1 ); //
	stages.push_back( new Pred_Table( opts ) );

	return;
}

Indirect::~Indirect(){
}

uint64_t Indirect::predict( uint64_t pc ){

	uint64_t target = 0;

	for( 	std::vector<Pred_Table*>::iterator it = stages.begin();
			it != stages.end();
			++it
	   )
	{
		if( (*it)->has_pc( pc ) ){
			target = (*it)->predict( pc );
		}else{
			(*it)->predict( pc );
		}
	}

	//defaults to current pc
	return target;
}

void Indirect::update( uint64_t pc, uint64_t target ){
	for( 	std::vector<Pred_Table*>::iterator it = stages.begin();
			it != stages.end();
			++it
	   )
	{
		if( target )
			(*it)->update( pc, target );
		else
			(*it)->update( pc );
	}

}

void Indirect::reset(){
	// TODO: clear stages.
}




