#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dram.h"

#define ROWBUF_SIZE         1024
#define DRAM_BANKS          16

//---- Latency for Part B ------

#define DRAM_LATENCY_FIXED  100

//---- Latencies for Part C ------

#define DRAM_T_ACT         45
#define DRAM_T_CAS         45
#define DRAM_T_PRE         45
#define DRAM_T_BUS         10


extern MODE   SIM_MODE;
extern uns64  CACHE_LINESIZE;

struct bank_info_strct
{
  uns64 some_open_row;
  uns64 row_open;
} bank_info[DRAM_BANKS] = {{0,0}};

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

DRAM   *dram_new(){
  DRAM *dram = (DRAM *) calloc (1, sizeof (DRAM));
  assert(DRAM_BANKS <= MAX_DRAM_BANKS);
  return dram;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void    dram_print_stats(DRAM *dram){
  double rddelay_avg=0;
  double wrdelay_avg=0;
  char header[256];
  sprintf(header, "DRAM");

  if(dram->stat_read_access){
    rddelay_avg=(double)(dram->stat_read_delay)/(double)(dram->stat_read_access);
  }

  if(dram->stat_write_access){
    wrdelay_avg=(double)(dram->stat_write_delay)/(double)(dram->stat_write_access);
  }

  printf("\n%s_READ_ACCESS\t\t : %10llu", header, dram->stat_read_access);
  printf("\n%s_WRITE_ACCESS\t\t : %10llu", header, dram->stat_write_access);
  printf("\n%s_READ_DELAY_AVG\t\t : %10.3f", header, rddelay_avg);
  printf("\n%s_WRITE_DELAY_AVG\t\t : %10.3f", header, wrdelay_avg);


}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

uns64   dram_access(DRAM *dram,Addr lineaddr, Flag is_dram_write){
  uns64 delay=DRAM_LATENCY_FIXED;

  if(SIM_MODE!=SIM_MODE_B){
    delay = dram_access_sim_rowbuf(dram, lineaddr, is_dram_write);
  }

  // Update stats
  if(is_dram_write){
    dram->stat_write_access++;
    dram->stat_write_delay+=delay;
  }else{
    dram->stat_read_access++;
    dram->stat_read_delay+=delay;
  }

  return delay;
}

///////////////////////////////////////////////////////////////////
// ------------ DO NOT MODIFY THE CODE ABOVE THIS LINE -----------
// Modify the function below only if you are attempting Part C
///////////////////////////////////////////////////////////////////

uns64   dram_access_sim_rowbuf(DRAM *dram,Addr lineaddr, Flag is_dram_write){
  uns64 delay=0;

  // Assume a mapping with consecutive lines in the same row
  // Assume a mapping with consecutive rowbufs in consecutive rows
  // You need to write this fuction to track open rows
  // You will need to compute delay based on row hit/miss/empty

  // Variables required
  uns64 bytes_per_rowbuf = ROWBUF_SIZE;
  uns64 bytes_per_line = CACHE_LINESIZE;
  uns64 no_of_banks = DRAM_BANKS;
  uns64 lines_per_row_buf = bytes_per_rowbuf / bytes_per_line;
  // uns64 col_idx = lineaddr % lines_per_row_buf;
  uns64 bank_idx = (lineaddr / lines_per_row_buf) % no_of_banks;
  uns64 row_idx = (((lineaddr / lines_per_row_buf)) / no_of_banks);

  //printf("%lld \n", bank_info[bank_idx].row_open);
  // if(is_dram_write != 0)
  // {
  //   dram->stat_write_access++;
  // }
  // else
  // {
  //   dram->stat_read_access++;
  // }

  if(bank_info[bank_idx].some_open_row == 0)
  {
    // no line open
    delay = DRAM_T_ACT + DRAM_T_CAS + DRAM_T_BUS;
    bank_info[bank_idx].some_open_row = 1;
    bank_info[bank_idx].row_open = row_idx;
    return delay;
  }

  else if(bank_info[bank_idx].some_open_row != 0 && \
     bank_info[bank_idx].row_open == row_idx)
  {
    // row row already open
    delay = DRAM_T_BUS + DRAM_T_CAS;
    return delay;
  }

  else
  {
    // some other row is open
    delay = DRAM_T_BUS + DRAM_T_PRE + DRAM_T_CAS + DRAM_T_ACT;
    bank_info[bank_idx].row_open = row_idx;
    return delay;
  }
}
