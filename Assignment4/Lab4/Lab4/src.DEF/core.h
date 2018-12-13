#ifndef CORE_H
#define CORE_H

#include "types.h"
#include "memsys.h"

typedef struct Core Core;



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


struct Core {
  uns   core_id;

  Memsys *memsys;
    
  char  trace_fname[1024];
  FILE *trace;
    
  uns   done;

  uns64  trace_inst_addr;
  uns64  trace_inst_type;
  uns64  trace_ldst_addr;
  
  uns64 snooze_end_cycle; // when waiting for data to return

  uns64 inst_count;
  uns64 done_inst_count;
  uns64 done_cycle_count;
};



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Core  *core_new(Memsys *memsys, char *trace_fname, uns core_id);
void   core_cycle(Core *core);
void   core_print_stats(Core *c);
void   core_read_trace(Core *c);
void   core_init_trace(Core *c);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif // CORE_H
