#ifndef _EXEQ_H_
#define _EXEQ_H_
#include <inttypes.h>
#include <assert.h>
#include "trace.h"
#include <cstdlib>

#define MAX_EXEQ_ENTRIES 16

extern int32_t LOAD_EXE_CYCLES;

typedef struct EXEQ_Entry_Struct {
  bool     valid;
  Inst_Info inst;
}EXEQ_Entry;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

typedef struct EXEQ {
  EXEQ_Entry  EXEQ_Entries[MAX_EXEQ_ENTRIES];
}EXEQ;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

EXEQ*     EXEQ_init(void);
void      EXEQ_print_state(EXEQ *t);
void      EXEQ_cycle(EXEQ *t);
void      EXEQ_insert(EXEQ *t, Inst_Info inst);
bool      EXEQ_check_done(EXEQ *t);
Inst_Info EXEQ_remove(EXEQ *t);

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#endif

