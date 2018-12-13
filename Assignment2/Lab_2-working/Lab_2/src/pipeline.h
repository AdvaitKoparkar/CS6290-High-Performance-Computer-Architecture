#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <inttypes.h>
#include <stdio.h>
#include <assert.h>

#include "trace.h"
#include "bpred.h"

#define MAX_PIPE_WIDTH 8


/*********************************************************************
* Pipeline Class & Internal Structures
**********************************************************************/


/* Pipeline Latches */
typedef struct Pipeline_Latch_Struct {
  bool valid;
  uint64_t op_id;
  bool stall;
  Trace_Rec tr_entry;
  bool is_mispred_cbr;
}Pipeline_Latch;

typedef enum Latch_Type_ENUM {
    FE_LATCH,
    ID_LATCH,
    EX_LATCH,
    MEM_LATCH,
    NUM_LATCH_TYPES
} Latch_Type;


typedef struct Pipeline {
  FILE *tr_file;
  Pipeline_Latch  pipe_latch[NUM_LATCH_TYPES][MAX_PIPE_WIDTH];// Pipeline Latches
  BPRED *b_pred;

  uint64_t op_id_tracker;         // a sequence number for OPs to track
  uint64_t halt_op_id;            // OpID of last inst in Trace
  bool halt;                      // Pipeline Done Flag

  bool fetch_cbr_stall;           // fetch stalled due to brach misprediction

  /* Statistics: students need to update these counters*/
  uint64_t stat_retired_inst;         // Total Commited Instructions
  uint64_t stat_num_cycle;            // Total Cycles
}Pipeline;

Pipeline* pipe_init(FILE *tr_file);   // Allocate Structures

void pipe_cycle(Pipeline *p);                        // Runs one Pipeline Cycle
void pipe_cycle_FE(Pipeline *p);                    // Fetch Stage
void pipe_cycle_ID(Pipeline *p);                    // ID Stage
void pipe_cycle_EX(Pipeline *p);                    // EX Stage
void pipe_cycle_MEM(Pipeline *p);                   // MEM Stage
void pipe_cycle_WB(Pipeline *p);                    // WB Stage

void pipe_check_bpred(Pipeline *p, Pipeline_Latch *fetch_op); // Branch Prediction Check

void pipe_print_state(Pipeline *p);                 // Print Pipeline Latches

// my code start
void print_pipeline_condition(Pipeline *p);
// my code end

#endif
