#ifndef _REST_H_
#define _REST_H_
#include <inttypes.h>
#include <assert.h>
#include <cstdlib>
#include "trace.h"

#define MAX_REST_ENTRIES 256


typedef struct REST_Entry_Struct {
  bool      valid;
  bool      scheduled;
  Inst_Info inst;
} REST_Entry;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

typedef struct REST {
  REST_Entry  REST_Entries[MAX_REST_ENTRIES];
} REST;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

REST* REST_init(void);
void  REST_print_state(REST *t);

bool  REST_check_space(REST *t);
void  REST_insert(REST *t, Inst_Info inst);
void  REST_remove(REST *t, Inst_Info inst);
void  REST_wakeup(REST *t, int tag);
void  REST_schedule(REST *t, Inst_Info inst);

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


#endif
