#ifndef _ROB_H_
#define _ROB_H_
#include <inttypes.h>
#include <assert.h>
#include "trace.h"
#include <cstdlib>

#define MAX_ROB_ENTRIES 256

typedef struct ROB_Entry_Struct {
  bool     valid;
  bool     ready;
  Inst_Info inst;
}ROB_Entry;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

typedef struct ROB {
  ROB_Entry  ROB_Entries[MAX_ROB_ENTRIES];
  int head_ptr;
  int tail_ptr;
}ROB;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

ROB*      ROB_init(void);
void      ROB_print_state(ROB *t);

bool      ROB_check_space(ROB *t);
int       ROB_insert(ROB *t, Inst_Info inst);
bool      ROB_check_ready(ROB *t, int tag);
void      ROB_mark_ready(ROB *t, Inst_Info inst);
bool      ROB_check_head(ROB *t);
Inst_Info ROB_remove_head(ROB *t);

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#endif

