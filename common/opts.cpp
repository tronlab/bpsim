/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#include "opts.h"

using namespace std;

Options::Options(){
	init();
}

Options::Options( const map<string,unsigned> &_opts ){
	init();
	for(	map<string,unsigned>::iterator it;
			it != _opts.end();
			++it							)
	{
		set( it->first, it->second );
	}
}

unsigned Options::operator[]( string str ){
	assert( opts.find(str) != opts.end() );
	return opts[str];
}

void Options::init(){

	string list[] = 
	{
		"hist_length",
		"index_bits",
		"pc_bits",
		"assoc_bits",
		"indirect",
		"is_tagged",
		"historesis",
		"counter_bits",
		"reverse_hist",
		"reverse_pc",
		"interleave",
		"hash_back",
	};

	vector<string> opt_list = vector<string>( list, list+sizeof(list)/sizeof(string) );

	for(	vector<string>::iterator it = opt_list.begin();
			it != opt_list.end();
			++it							)
	{
		opts[*it] = 0;
	}
}


void Options::set( string str, unsigned i ){
	assert( opts.find(str) != opts.end() );
	opts[str] = i;
}

void Options::check_invariants(){
	if( opts["indirect"] ){
		if( opts["is_tagged"] )
			assert( opts["interleave"] ); //it would be silly not too
	}else{
		assert( opts["counter_bits"] );
	}

	if( opts["assoc_bits"] )
		assert( opts["is_tagged"] );

	assert( opts["pc_bits"] <= opts["index_bits"] );
	assert( opts["pc_bits"] > 0 );

}

void Options::print(){
	for(	map<string,unsigned>::iterator it = opts.begin();
			it != opts.end();
			++it							)
	{
		cout << it->first << "\t=" << it->second << endl;
	}
}


