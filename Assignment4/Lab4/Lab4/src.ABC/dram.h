#ifndef DRAM_H
#define DRAM_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

#define MAX_DRAM_BANKS          256



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

typedef struct DRAM   DRAM;
typedef struct Rowbuf_Entry Rowbuf_Entry;


struct Rowbuf_Entry {
  Flag valid; // 0 means the rowbuffer entry is invalid
  uns64 rowid; // If the entry is valid, which row?
};


struct DRAM {
  Rowbuf_Entry perbank_row_buf[MAX_DRAM_BANKS];
  
   // stats 
  uns64 stat_read_access;
  uns64 stat_write_access;
  uns64 stat_read_delay;
  uns64 stat_write_delay;
};



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

DRAM   *dram_new();
void    dram_print_stats(DRAM *dram);
uns64   dram_access(DRAM *dram,Addr lineaddr, Flag is_dram_write);
uns64   dram_access_sim_rowbuf(DRAM *dram,Addr lineaddr, Flag is_dram_write);




#endif // DRAM_H
