/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#ifndef HASH_H
#define HASH_H

#include <string>
#include <deque>
#include <cassert>
#include <string>
#include <iostream>
#include <stdint.h>

#include "opts.h"


#include <iomanip>

class Hash {
	Options &opts;


	std::string bin_cond_hist( const std::deque<bool>& hist );
	std::string bin_num( uint64_t num );

	inline uint64_t get_pc_bits( uint64_t pc );
	inline uint64_t create_mask( int bits );
	inline uint64_t reverse( uint64_t value, int bits );

  public:

	Hash( Options &_opts );
	//~Hash();

	uint64_t operator()( const uint64_t pc, const std::deque<uint64_t>& hist );
	uint64_t operator()( const uint64_t pc, const std::deque<bool>& hist );

	uint64_t interleave( const uint64_t pc, const std::deque<uint64_t>& hist );
	uint64_t fold( const uint64_t pc, const std::deque<bool>& hist );

	uint64_t hash_back( const uint64_t pc, const std::deque<bool>& hist );

	void print_cond_hist( const std::deque<bool>& hist );
	void print_ind_hist( const std::deque<uint64_t>& hist );
};



#endif /* ifndef HASH_H */

