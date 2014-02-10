/* Author: Tom Manville
 * tdmanv@umich.edu
 * 2012
 *
 * Please see LICENSE for license information.
 *
 * Based on work by:
 * Author: Hongliang Gao;   Created: Jan 27 2011
 */

#ifndef CBP3_DEF_H_INCLUDED
#define CBP3_DEF_H_INCLUDED

#define NUM_REGS 121
#define PIPE_WIDTH 4
// each uop has up to 6 src registers
// function reg_is_constant can be used to tell whether a src is a real register
// or a constant value
#define UOP_SRCS_MAX 6
// the framework provides timing info of six stages:
// fetch, allocation, execution, agu, std (store data is ready) and retire
#define NUM_STAGES 6

// uop type encoding
#define IS_BR_CONDITIONAL (1 << 0)   // conditional branch
#define IS_BR_INDIRECT    (1 << 1)   // indirect branch
#define IS_BR_CALL        (1 << 2)   // call
#define IS_BR_RETURN      (1 << 3)   // return
#define IS_BR_OTHER       (1 << 4)   // other branches
#define IS_LOAD           (1 << 5)   // load uop
#define IS_STORE          (1 << 6)   // store uop
#define IS_FP             (1 << 7)   // floating point uop
#define IS_WFLAGS         (1 << 8)   // uop writes flags
#define IS_RFLAGS         (1 << 9)   // uop reads flags
#define IS_BOM            (1 << 14)  // uop is the beginning uop of an instruction
#define IS_EOM            (1 << 15)  // uop is the end uop of an instruction

// register type
typedef enum {
    REG_ZERO,  // constant zero
    REG_ONES,  // constant ones
    REG_IMM,   // immediate value
    REG_INT,   // integer register
    REG_SEG,   // segment register
    REG_FP,    // floating point register
    REG_OTHER  // control register etc.
}REG_TYPE;

// some of register names that may be useful:
//             Register Name      Register Number
//                  RSP                7
//                  RBP                8
//                  RSI                9
//                  RDI               10

// some helper functions
REG_TYPE get_reg_type(uint8_t r);     // convert register number to type
bool     reg_is_constant(uint8_t r);  // check whether a register is REG_ZERO, REG_ONES or REG_IMM
bool     uop_is_branch(int16_t type); // check whether an uop is a branch

// dynamic and static info of an uop
class cbp3_uop_dynamic_t {
    public:
        cbp3_uop_dynamic_t (){ reset(); }
        void reset(){}

        uint64_t br_target;                 // real branch target
        int64_t     br_taken;               // real branch direction
        uint16_t type;                      // uop type ( refer to IS_* encodings defined above)
        uint64_t pc;                        // address of an instruction
        
};

class cbp3_queue_entry_t {
    public:
        cbp3_queue_entry_t () { reset(); }
        void reset();

        bool valid;

        // dynamic info of the uop
        cbp3_uop_dynamic_t uop;

        // cycle info of each stage
        uint32_t cycle_fetch;          // fetch
        uint32_t cycle_allocate;       // allocation from fetch queue to rob
        uint32_t cycle_agu;            // address generation
        uint32_t cycle_std;            // store data is ready
        uint32_t cycle_exe;            // execution
        uint32_t cycle_retire;         // retire

        uint32_t cycle_last_pred;      // cycle of the last prediction reported for this uop
        uint32_t target_pred;            // last prediction of a branch
        int32_t taken_pred;            // last prediction of a branch
};

#endif
