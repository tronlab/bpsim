/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see top level directory for license.
 */

#ifndef INDIRECT_H
#define INDIRECT_H

#include <vector>
#include <iostream>
#include <cassert>
#include <stdint.h>

#include "opts.h"
#include "pred_table.h"

class Indirect {
	std::vector<Pred_Table*> stages;
  public:
	Indirect();
	~Indirect();
	uint64_t predict( uint64_t pc );
	void update( uint64_t pc, uint64_t taken);
	void reset();
	void finish(){}
};


#endif /* ifndef INDIRECT_H */

