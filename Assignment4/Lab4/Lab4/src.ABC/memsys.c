#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "memsys.h"


//---- Cache Latencies  ------

#define DCACHE_HIT_LATENCY   1
#define ICACHE_HIT_LATENCY   1
#define L2CACHE_HIT_LATENCY  10

extern MODE   SIM_MODE;
extern uns64  CACHE_LINESIZE;
extern uns64  REPL_POLICY;

extern uns64  DCACHE_SIZE;
extern uns64  DCACHE_ASSOC;
extern uns64  ICACHE_SIZE;
extern uns64  ICACHE_ASSOC;
extern uns64  L2CACHE_SIZE;
extern uns64  L2CACHE_ASSOC;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


Memsys *memsys_new(void)
{
  Memsys *sys = (Memsys *) calloc (1, sizeof (Memsys));

  sys->dcache = cache_new(DCACHE_SIZE, DCACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);

  if(SIM_MODE!=SIM_MODE_A){
    sys->icache = cache_new(ICACHE_SIZE, ICACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
    sys->l2cache = cache_new(L2CACHE_SIZE, L2CACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
    sys->dram    = dram_new();
  }

  return sys;

}


////////////////////////////////////////////////////////////////////
// This function takes an ifetch/ldst access and returns the delay
////////////////////////////////////////////////////////////////////

uns64 memsys_access(Memsys *sys, Addr addr, Access_Type type, uns core_id)
{
  uns delay=0;


  // all cache transactions happen at line granularity, so get lineaddr
  Addr lineaddr=addr/CACHE_LINESIZE;


  if(SIM_MODE==SIM_MODE_A){
    delay = memsys_access_modeA(sys,lineaddr,type,core_id);
  }else{
    delay = memsys_access_modeBC(sys,lineaddr,type,core_id);
  }


  //update the stats
  if(type==ACCESS_TYPE_IFETCH){
    sys->stat_ifetch_access++;
    sys->stat_ifetch_delay+=delay;
  }

  if(type==ACCESS_TYPE_LOAD){
    sys->stat_load_access++;
    sys->stat_load_delay+=delay;
  }

  if(type==ACCESS_TYPE_STORE){
    sys->stat_store_access++;
    sys->stat_store_delay+=delay;
  }


  return delay;
}



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void memsys_print_stats(Memsys *sys)
{
  char header[256];
  sprintf(header, "MEMSYS");

  double ifetch_delay_avg=0;
  double load_delay_avg=0;
  double store_delay_avg=0;

  if(sys->stat_ifetch_access){
    ifetch_delay_avg = (double)(sys->stat_ifetch_delay)/(double)(sys->stat_ifetch_access);
  }

  if(sys->stat_load_access){
    load_delay_avg = (double)(sys->stat_load_delay)/(double)(sys->stat_load_access);
  }

  if(sys->stat_store_access){
    store_delay_avg = (double)(sys->stat_store_delay)/(double)(sys->stat_store_access);
  }


  printf("\n");
  printf("\n%s_IFETCH_ACCESS  \t\t : %10llu",  header, sys->stat_ifetch_access);
  printf("\n%s_LOAD_ACCESS    \t\t : %10llu",  header, sys->stat_load_access);
  printf("\n%s_STORE_ACCESS   \t\t : %10llu",  header, sys->stat_store_access);
  printf("\n%s_IFETCH_AVGDELAY\t\t : %10.3f",  header, ifetch_delay_avg);
  printf("\n%s_LOAD_AVGDELAY  \t\t : %10.3f",  header, load_delay_avg);
  printf("\n%s_STORE_AVGDELAY \t\t : %10.3f",  header, store_delay_avg);
  printf("\n");

  cache_print_stats(sys->dcache, "DCACHE");

  if(SIM_MODE!=SIM_MODE_A){
    cache_print_stats(sys->icache, "ICACHE");
    cache_print_stats(sys->l2cache, "L2CACHE");
    dram_print_stats(sys->dram);
  }

}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

uns64 memsys_access_modeA(Memsys *sys, Addr lineaddr, Access_Type type, uns core_id){
  Flag needs_dcache_access=FALSE;
  Flag is_write=FALSE;

  if(type == ACCESS_TYPE_IFETCH){
    // no icache in this mode
  }

  if(type == ACCESS_TYPE_LOAD){
    needs_dcache_access=TRUE;
    is_write=FALSE;
  }

  if(type == ACCESS_TYPE_STORE){
    needs_dcache_access=TRUE;
    is_write=TRUE;
  }

  if(needs_dcache_access){
    Flag outcome=cache_access(sys->dcache, lineaddr, is_write,core_id);
    if(outcome==MISS){
      cache_install(sys->dcache, lineaddr, is_write,core_id);
    }
  }

  // timing is not simulated in Part A
  return 0;
}

////////////////////////////////////////////////////////////////////
// --------------- DO NOT CHANGE THE CODE ABOVE THIS LINE ----------
////////////////////////////////////////////////////////////////////

uns64 memsys_access_modeBC(Memsys *sys, Addr lineaddr, Access_Type type,uns core_id)
{
  uns64 delay=0;
  Flag needs_dcache_access = FALSE;
  Flag needs_icache_access = FALSE;
  Flag is_write = FALSE;

  if(type == ACCESS_TYPE_IFETCH)
  {
    needs_icache_access = TRUE;
    is_write = FALSE;
  }

  if(type == ACCESS_TYPE_LOAD)
  {
    needs_dcache_access=TRUE;
    is_write=FALSE;
  }

  if(type == ACCESS_TYPE_STORE){
    needs_dcache_access=TRUE;
    is_write=TRUE;
  }

  if(needs_icache_access == TRUE)
  {
      uns cache_write = 0;
      Flag icache_outcome = cache_access(sys->icache, lineaddr, cache_write, core_id);

      // if icache HIT
      if(icache_outcome == HIT)
      {
        delay = ICACHE_HIT_LATENCY;
        return delay;
      }

      // if icache_miss
      else
      {
        delay = ICACHE_HIT_LATENCY + \
                memsys_L2_access(sys, lineaddr, cache_write, core_id);

        // put new instruction into icache
        cache_install(sys->icache, lineaddr, cache_write, core_id);

        return delay;
      }
  }

  else if(needs_dcache_access == TRUE)
  {
      uns cache_write = is_write;
      Flag dcache_outcome = cache_access(sys->dcache, lineaddr, cache_write, core_id);

      // if dcache HIT
      if(dcache_outcome == HIT)
      {
        delay = DCACHE_HIT_LATENCY;
        return delay;
      }

      // if dcache_miss
      else
      {
        delay = DCACHE_HIT_LATENCY + \
                memsys_L2_access(sys, lineaddr, 0, core_id);

        // put new instruction into dcache
        cache_install(sys->dcache, lineaddr, cache_write, core_id);

        // check if evict occured and it has dirty bit set
        if(sys->dcache->last_evicted_line.valid != 0 && \
           sys->dcache->last_evicted_line.dirty != 0)
        // if(sys->dcache->last_evicted_line.valid != 0)
        {
          uns64 last_evict_addr = sys->dcache->last_evicted_line.tag * sys->dcache->num_sets;
          last_evict_addr += lineaddr % sys->dcache->num_sets; // set_idx

          // don't add to delay - parallel writeback
          memsys_L2_access(sys, last_evict_addr, \
                                    sys->dcache->last_evicted_line.dirty, \
                                    core_id);

          sys->dcache->last_evicted_line.valid = 0;
        }

        return delay;
      }
  }


  return delay;
}


/////////////////////////////////////////////////////////////////////
// This function is called on ICACHE miss, DCACHE miss, DCACHE writeback
// ----- YOU NEED TO WRITE THIS FUNCTION AND UPDATE DELAY ----------
/////////////////////////////////////////////////////////////////////

uns64   memsys_L2_access(Memsys *sys, Addr lineaddr, Flag is_writeback, uns core_id)
{
  uns64 delay = 0;
  uns64 dirtyaddr=0;

  //To get the delay of L2 MISS, you must use the dram_access() function
  //To perform writebacks to memory, you must use the dram_access() function
  //This will help us track your memory reads and memory writes

  Flag is_hit = cache_access(sys->l2cache, lineaddr, is_writeback, core_id);
  if(is_hit == HIT)
  {
    delay = L2CACHE_HIT_LATENCY;
    return delay;
  }
  else
  {
    // update delay
    delay = L2CACHE_HIT_LATENCY + dram_access(sys->dram, lineaddr, 0);

    // put block in l2 cache
    cache_install(sys->l2cache, lineaddr, is_writeback, core_id);

    // check if evicted is dirty
    if(sys->l2cache->last_evicted_line.valid != 0 && \
       sys->l2cache->last_evicted_line.dirty != 0)
    {
      dirtyaddr = sys->l2cache->last_evicted_line.tag * sys->l2cache->num_sets;
      dirtyaddr += lineaddr % sys->l2cache->num_sets;
      dram_access(sys->dram, dirtyaddr, 1);

      sys->l2cache->last_evicted_line.valid = 0;

    }

  }

  return delay;
}
