#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "core.h"

extern uns64 cycle;

extern void die_message(const char * msg);


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

Core *core_new(Memsys *memsys, char *trace_fname, uns core_id)
{
  Core *c = (Core *) calloc (1, sizeof (Core));
  c->core_id = core_id;
  c->memsys  = memsys;

  strcpy(c->trace_fname, trace_fname);
  core_init_trace(c);
  core_read_trace(c);

  return c;
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void core_init_trace(Core *c)
{
  char  command_string[512];
  
  sprintf(command_string,"gunzip -c %s", c->trace_fname);
  if ((c->trace = popen(command_string, "r")) == NULL){
    printf("Command string is %s\n", command_string);
    die_message("Unable to open the file with gzip option \n");
  }
  
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void core_cycle (Core *c)
{
  if(c->done){
    return;
  }

  // if core is snoozing on DRAM hits, return ..
  if(cycle <= c->snooze_end_cycle){
      return;
  }

  c->inst_count++;

  uns ifetch_delay=0, ld_delay=0, st_delay=0, bubble_cycles=0;
	
  ifetch_delay = memsys_access(c->memsys, c->trace_inst_addr, ACCESS_TYPE_IFETCH, c->core_id);
  if(ifetch_delay>1){
    bubble_cycles += (ifetch_delay-1);
  }

  if(c->trace_inst_type==INST_TYPE_LOAD){
    ld_delay = memsys_access(c->memsys, c->trace_ldst_addr, ACCESS_TYPE_LOAD, c->core_id);
  }
  if(ld_delay>1){
    bubble_cycles += (ld_delay-1);
  }
  
  if(c->trace_inst_type==INST_TYPE_STORE){
    st_delay = memsys_access(c->memsys, c->trace_ldst_addr, ACCESS_TYPE_STORE, c->core_id);
  }
  //No bubbles for store misses


  if(bubble_cycles){
    c->snooze_end_cycle = (cycle+bubble_cycles);
  }

  core_read_trace(c);
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void core_read_trace (Core *c){
  uns tmp;
  tmp = fread (&c->trace_inst_addr, 4, 1, c->trace);
  tmp = fread (&c->trace_inst_type, 1, 1, c->trace);
  tmp = fread (&c->trace_ldst_addr, 4, 1, c->trace);
  
  if(feof(c->trace)){
    c->done=TRUE;
    c->done_inst_count  = c->inst_count;
    c->done_cycle_count = cycle;
  }
  
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void core_print_stats(Core *c)
{
  char header[256];
  double ipc = (double)(c->done_inst_count)/(double)(c->done_cycle_count);
  sprintf(header, "CORE_%01d", c->core_id);
  
  printf("\n");
  printf("\n%s_INST         \t\t : %10llu", header,  c->done_inst_count);
  printf("\n%s_CYCLES       \t\t : %10llu", header,  c->done_cycle_count);
  printf("\n%s_IPC          \t\t : %10.3f", header,  ipc);

  pclose(c->trace);
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
