#ifndef _TRACE_H
#define _TRACE_H

#include <stdio.h>
#include <iostream>
#include <inttypes.h>

typedef enum Op_Type_Enum{
    OP_ALU,             // ALU(ADD/ SUB/ MUL/ DIV) operaiton
    OP_LD,              // load operation
    OP_ST,              // store operation
    OP_CBR,             // Conditional Branch
    OP_OTHER,           // Other Ops
    NUM_OP_TYPE
} Op_Type;

/* Data structure for Trace Record */
typedef struct Trace_Rec_Struct {
    uint64_t inst_addr;  // instruction address
    uint8_t  op_type;    // optype
    uint8_t  dest;       // Destination
    uint8_t  dest_needed; //
    uint8_t  src1_reg;       // Source Register 1
    uint8_t  src2_reg;       // Source Register 2
    uint8_t  src1_needed; // Source Register 1 needed by this instruction
    uint8_t  src2_needed; // Source Register 2 needed to this instruction
    uint8_t  cc_read;    // Conditional Code Read
    uint8_t  cc_write;   // Conditional Code Write
    uint64_t mem_addr;   // Load / Store Memory Address
    uint8_t  mem_write;  // Write
    uint8_t  mem_read;   // Read
    uint8_t  br_dir;     // Branch Direction Taken / Not Taken
    uint64_t br_target;  // Target Address of Branch
} Trace_Rec;

#endif
