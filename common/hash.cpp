/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#include "hash.h"

using namespace std;

template<typename T>
void printBin(const T& t){
    size_t nBytes=sizeof(T);
    char* rawPtr((char*)(&t));
    for(size_t byte=0; byte<nBytes; byte++){
        for(size_t bit=0; bit<8; bit++){
            std::cout<<(((rawPtr[byte])>>bit)&1);
        }
        std::cout<<"-";
    }
    std::cout<<std::endl;
};

string Hash::bin_cond_hist( const std::deque<bool>& hist ){
	string str( "" );
	for( unsigned i=0; i<hist.size(); i++ ){
		if( hist[i] )
			str = "1" + str;
		else
			str = "0" + str;
	}
	return str;
}

string Hash::bin_num( uint64_t num ){
	string str( "" );
	for( uint64_t i=0; i<8*sizeof(uint64_t); i++ ){
		if( num & ((uint64_t)1<<i) )
			str = "1" + str;
		else
			str = "0" + str;
	}
	return str;
}

void Hash::print_cond_hist( const std::deque<bool>& hist ){
	cout << bin_cond_hist( hist ) << endl;
}

void Hash::print_ind_hist( const std::deque<uint64_t>& hist ){
	for( unsigned i=0; i<hist.size(); i++ ){
		cout << bin_num( hist[i] ) << endl;
	}
	cout << endl;
}

Hash::Hash( Options &_opts ): opts(_opts){
}

#define PC_SHIFT 3
uint64_t Hash::get_pc_bits( uint64_t pc ){

	uint64_t pc_bits = (pc>>PC_SHIFT);

	pc_bits = pc_bits & create_mask( opts["pc_bits"] );
	/*
	if( !opts["indirect"] )
		cout << hex << pc_bits << endl << endl;
		*/

	if( opts["reverse_pc"] ){
		pc_bits = reverse( pc_bits, opts["pc_bits"] );
	}
	return pc_bits;
}

uint64_t Hash::operator()( const uint64_t pc, const deque<uint64_t>& hist ){
	assert( opts["indirect"] );

	uint64_t pc_bits = get_pc_bits( pc );

	return interleave( pc_bits, hist );
}

uint64_t Hash::operator()( const uint64_t pc, const deque<bool>& hist ){
	assert( ! opts["indirect"] );

	uint64_t pc_bits = get_pc_bits( pc );

	if( opts["hist_length"] == 0 ){
		return pc_bits;
	}


	uint64_t ret;
	if( opts["hash_back"] )
		ret = hash_back( pc_bits, hist );
	else
		ret = fold( (uint64_t) pc_bits, hist );

	/*
	cout << opts["hist_length"] << endl;
	printBin( (uint16_t) (pc>>PC_SHIFT) );
	print_cond_hist( hist );
	printBin( (uint16_t) ret );
	cout << endl;

	if( opts["hist_length"] == 3 )
		assert(0);
		*/
	if( opts["hist_length"] == 8 ){
		//cout << ret << endl;
	}
	if( opts["hist_length"] == 3 ){
		//cout << ret << endl;
	}

	return ret;
}

inline uint64_t Hash::create_mask( int bits ){
	return ( 1 << bits ) - 1;
}

inline uint64_t Hash::reverse( uint64_t value, int bits ){
	uint64_t result = 0;
	for( int i=0; i<bits; ++i ){
		int j = bits - i - 1;
		if( value & (1<<i) )
			result = result | (1<<j);
	}
	return result;
}

uint64_t Hash::hash_back( const uint64_t pc_bits, const deque<bool>& hist ){
	uint64_t hist_bits = 0;
	for( unsigned i=0; i<hist.size(); ++i){
		if( hist[i] ){
			hist_bits	^= (1<<(i%opts["index_bits"]));
		}
	}
	return pc_bits ^ hist_bits;
}

uint64_t Hash::fold( uint64_t pc, const deque<bool>& hist ){

	uint64_t pc_bits=pc;
	uint64_t result=0;
	uint64_t hist_bits=0;

	//ignore lowest 2 bits; byte addressable memory


	for( uint64_t i=0; i<opts["pc_bits"]; i++ ){
		if( pc_bits & (1<<i) )
			result = result ^ (1<<(i%opts["index_bits"]));
	}

	unsigned idx = opts["pc_bits"];
	for( unsigned i=0; i<hist.size(); ++i){
		if( hist[i] ){
			hist_bits	^= (1<<i);
			result		^= (1<<(idx%opts["index_bits"]));
		}
		++idx;
	}

	return result;
}

uint64_t Hash::interleave( uint64_t pc, const deque<uint64_t>& hist ){
	assert( hist.size() == opts["hist_length"] );

	//the lowest bit is the first thing in hist
	uint64_t result = pc;
	//cout << bin_num(result) << endl;

	for( unsigned j=0; j<8*sizeof(hist[0]); j++ ){
		for( unsigned i=0; i<hist.size(); i++ ){

			uint64_t bit;
			//select bit from history
			bit = hist[i] & ((uint64_t)1<<j);
			//move bit up 
			bit = bit << ( i+j*hist.size() );
			//cout << bin_num(bit) << endl;
			result = result ^ bit;
			//cout << bin_num(result) << endl;
		}
	}
	return result%(1<<opts["index_bits"]);
}


