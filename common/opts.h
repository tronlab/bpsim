/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#ifndef OPTS_H
#define OPTS_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cassert>
//#include <stdint.h>

/*
*/


class Options{

	std::map<std::string,unsigned> opts;
	void init();

  public:

	Options();
	Options( const std::map<std::string,unsigned> &_opts );

	unsigned operator[]( std::string );
	void set( std::string, unsigned );
	void check_invariants();
	void print();

};


#endif /* ifndef OPTS_H */

