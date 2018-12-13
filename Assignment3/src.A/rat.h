#ifndef _RAT_H_
#define _RAT_H_
#include <inttypes.h>
#include <cstdlib>
#define MAX_ARF_REGS 32

typedef struct RAT_Entry_Struct {
  bool     valid;
  uint64_t prf_id;
}RAT_Entry;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

typedef struct RAT {
  RAT_Entry  RAT_Entries[MAX_ARF_REGS];
}RAT;

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

RAT* RAT_init(void);
void RAT_print_state(RAT *t);

int  RAT_get_remap(RAT *t, int arf_id);
void RAT_set_remap(RAT *t, int arf_id, int prf_id);
void RAT_reset_entry(RAT *t, int arf_id);


#endif

