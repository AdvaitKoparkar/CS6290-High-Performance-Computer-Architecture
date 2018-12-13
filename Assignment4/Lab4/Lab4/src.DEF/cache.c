#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "cache.h"

extern uns64 SWP_CORE0_WAYS;
extern uns64 cycle; // You can use this as timestamp for LRU
extern MODE   SIM_MODE;
uns64 fSWP_CORE0_WAYS;
uns64 nsets_UCP = 32;
extern uns64  L2CACHE_SIZE;
extern uns64  L2CACHE_ASSOC;
extern uns64  CACHE_LINESIZE;


UMON *UMON_new(uns64 nways, uns64 msets, uns64 linesize)
{
  UMON *U = (UMON*)calloc(1, sizeof(UMON));
  U->nways = nways;
  U->msets = msets;
  U->linesize = linesize;
  U->repl_policy = 0;
  U->misscount = 0;

  U->ATD = cache_new(nways*msets*linesize, nways, linesize, U->repl_policy);
  U->ctr = (uns64*) calloc(nways, sizeof(uns64));
  uns i = 0;
  for(i = 0; i < nways; i++)
  {
    U->ctr[i] = 0;
  }

  return U;
}


////////////////////////////////////////////////////////////////////
// ------------- DO NOT MODIFY THE INIT FUNCTION -----------
////////////////////////////////////////////////////////////////////

Cache  *cache_new(uns64 size, uns64 assoc, uns64 linesize, uns64 repl_policy){

   Cache *c = (Cache *) calloc (1, sizeof (Cache));
   c->num_ways = assoc;
   c->repl_policy = repl_policy;

   if(c->num_ways > MAX_WAYS){
     printf("Change MAX_WAYS in cache.h to support %llu ways\n", c->num_ways);
     exit(-1);
   }

   // determine num sets, and init the cache
   c->num_sets = size/(linesize*assoc);
   c->sets  = (Cache_Set *) calloc (c->num_sets, sizeof(Cache_Set));

   if(c->repl_policy==3)
   {
     c->umon[0] = UMON_new(assoc, size/(assoc*nsets_UCP), linesize);
     c->umon[1] = UMON_new(assoc, size/(assoc*nsets_UCP), linesize);
   }
   c->ATD_install = 0;

   return c;
}

////////////////////////////////////////////////////////////////////
// ------------- DO NOT MODIFY THE PRINT STATS FUNCTION -----------
////////////////////////////////////////////////////////////////////

void    cache_print_stats    (Cache *c, char *header){
  double read_mr =0;
  double write_mr =0;

  if(c->stat_read_access){
    read_mr=(double)(c->stat_read_miss)/(double)(c->stat_read_access);
  }

  if(c->stat_write_access){
    write_mr=(double)(c->stat_write_miss)/(double)(c->stat_write_access);
  }

  printf("\n%s_READ_ACCESS    \t\t : %10llu", header, c->stat_read_access);
  printf("\n%s_WRITE_ACCESS   \t\t : %10llu", header, c->stat_write_access);
  printf("\n%s_READ_MISS      \t\t : %10llu", header, c->stat_read_miss);
  printf("\n%s_WRITE_MISS     \t\t : %10llu", header, c->stat_write_miss);
  printf("\n%s_READ_MISSPERC  \t\t : %10.3f", header, 100*read_mr);
  printf("\n%s_WRITE_MISSPERC \t\t : %10.3f", header, 100*write_mr);
  printf("\n%s_DIRTY_EVICTS   \t\t : %10llu", header, c->stat_dirty_evicts);

  printf("\n");
}



////////////////////////////////////////////////////////////////////
// Note: the system provides the cache with the line address
// Return HIT if access hits in the cache, MISS otherwise
// Also if is_write is TRUE, then mark the resident line as dirty
// Update appropriate stats
////////////////////////////////////////////////////////////////////

Flag cache_access(Cache *c, Addr lineaddr, uns is_write, uns core_id)
{
  Flag outcome=MISS;
  // Your Code Goes Here
  // my code start

  //////////////////////////////////////////////////////////////////////////////
  // My test space
  // printf("0x%llx\n", lineaddr); // There is no block addr
  // printf("Num Sets %lld\n", c->num_sets);
  // printf("Num ways %lld\n", c->num_ways);
  // uns32 idx = (lineaddr % c->num_sets) & (c->num_sets-1); // first
  //                                                         // log2(num_sets) are
  //                                                         // the index
  // printf("Index %d\n", idx);
  // uns32 tag = (lineaddr) / c->num_sets;                   // remaining tag
  // printf("Tag %d\n", tag);
  //////////////////////////////////////////////////////////////////////////////

  // Variables needed
  uns32 way_ind;
  uns32 idx = (lineaddr % c->num_sets);
  // Variables for F
  uns i, j;
  int ATD_way = -1;
  uns64 cyc = 0;
  uns64 cycarr[c->num_ways];
  int pos = -1;
  uns swptmp;

  // PART F - Monitor
  if(c->repl_policy == 3)
  {
    if(idx % nsets_UCP == 0)
    {
      idx = idx / nsets_UCP;
      // check if set in ATD
      for(i=0; i < c->umon[core_id]->nways; i++)
      {
        if(c->umon[core_id]->ATD->sets[idx].line[i].valid != 0 && \
           c->umon[core_id]->ATD->sets[idx].line[i].tag == \
           (lineaddr/c->num_sets))
        {
          cyc = c->umon[core_id]->ATD->sets[idx].line[i].last_access_time;
          ATD_way = i;
        }
        cycarr[i] = c->umon[core_id]->ATD->sets[idx].line[i].last_access_time;
      }

      for(i=0; i < c->umon[core_id]->nways-1; i++)
      {
        for(j=0; j < c->umon[core_id]->nways-i-1; j++)
        {
          if(cycarr[j] < cycarr[j+1])
          {
            swptmp = cycarr[j];
            cycarr[j] = cycarr[j+1];
            cycarr[j+1] = swptmp;
          }
        }
      }
      for(i = 0; i < c->umon[core_id]->nways; i++)
      {
        if(cycarr[i] == cyc)
        {
          pos = i;
        }
      }

      // Hit in ATD
      if(ATD_way != -1)
      {
        // printf("POS %lld\n", pos);
        // if(pos == 2)
        //   exit(1);
        c->umon[core_id]->ctr[pos]++;
      }
      else
      {
        // ATD install
        c->umon[core_id]->ATD->ATD_install = 1;
        cache_install(c->umon[core_id]->ATD, lineaddr, is_write, core_id);
        c->umon[core_id]->ATD->ATD_install = 0;
        c->umon[core_id]->misscount++;
      }
      idx = idx * nsets_UCP;
    }

  }


  for(way_ind = 0; way_ind < c->num_ways; way_ind++)
  {
    if(c->sets[idx].line[way_ind].valid != 0 && \
       c->sets[idx].line[way_ind].tag == (lineaddr) / c->num_sets)
    {
      if(is_write != 0)
      {
        // stat_write_access++;
        c->sets[idx].line[way_ind].dirty = 1;
      }
      else
      {
        // stat_read_access++;
      }
      // Update last access element
      c->sets[idx].line[way_ind].last_access_time = cycle;
      outcome = HIT;
      // return outcome;
    }
  }

  // Update access count
  if(is_write != 0)
  {
    c->stat_write_access++;
  }
  else
  {
    c->stat_read_access++;
  }

  // No tag match = MISS
  // Update miss count
  if(is_write != 0 && outcome == MISS)
  {
    c->stat_write_miss++;
  }
  if(is_write == 0 && outcome == MISS)
  {
    c->stat_read_miss++;
  }

  // my code end
  return outcome;
}

////////////////////////////////////////////////////////////////////
// Note: the system provides the cache with the line address
// Install the line: determine victim using repl policy (LRU/RAND)
// copy victim into last_evicted_line for tracking writebacks
////////////////////////////////////////////////////////////////////

void cache_install(Cache *c, Addr lineaddr, uns is_write, uns core_id)
{
  // Your Code Goes Here
  // Find victim using cache_find_victim
  // Initialize the evicted entry
  // Initialize the victime entry

  // Variables needed
  int32 vic_ind = 0;
  uns set_idx = (lineaddr % c->num_sets) & (c->num_sets-1);
  uns32 tag = (lineaddr) / c->num_sets;

  if(c->ATD_install == 1)
  {
    set_idx = set_idx / nsets_UCP;
    tag = (lineaddr)/(L2CACHE_SIZE/(L2CACHE_ASSOC*CACHE_LINESIZE));
  }

  if(set_idx >= c->num_sets)
  {
    printf("Idx error\n");
    exit(0);
  }

  // find victim
  vic_ind = cache_find_victim(c, set_idx, core_id);

  if(c->sets[set_idx].line[vic_ind].valid != 0)
  {
    // Check dirty bit
    if(c->sets[set_idx].line[vic_ind].dirty != 0)
    {
      c->stat_dirty_evicts++;
      // need to write to mem? Happens in bg maybe
    }

    // Update last evicted line
    c->last_evicted_line = c->sets[set_idx].line[vic_ind];
  }

  // replace line
  c->sets[set_idx].line[vic_ind].valid = 1;
  if(is_write)
  {
    c->sets[set_idx].line[vic_ind].dirty = 1;
  }
  else
  {
    c->sets[set_idx].line[vic_ind].dirty = 0;
  }
  c->sets[set_idx].line[vic_ind].tag = tag;
  c->sets[set_idx].line[vic_ind].core_id = core_id;
  c->sets[set_idx].line[vic_ind].last_access_time = cycle;
  return;
}

////////////////////////////////////////////////////////////////////
// You may find it useful to split victim selection from install
////////////////////////////////////////////////////////////////////


uns cache_find_victim(Cache *c, uns set_index, uns core_id){
  // Variables needed
  uns earliest_time = 1000000000;
  int32 earliest_ind = 0;
  uns victim=-1;
  uns LRU_way_ind;
  uns set_it;
  uns occ0 = 0;
  uns occ1 = 0;
  uns q0 = 0;
  uns q1 = 0;
  if(c->repl_policy == 2)
  {
    q0 = SWP_CORE0_WAYS;
    q1 = c->num_ways - SWP_CORE0_WAYS;
  }
  else if(c->repl_policy == 3)
  {
    q0 = fSWP_CORE0_WAYS;
    q1 = c->num_ways - fSWP_CORE0_WAYS;
  }
  uns64 oldest0 = 10000000000000;
  uns64 oldest1 = 10000000000000;
  uns old_ind0 = 0;
  uns old_ind1 = 0;
  uns it = 0;


  for(set_it = 0; set_it < c->num_ways; set_it++)
  {
    if(c->sets[set_index].line[set_it].valid == 0)
    {
      victim = set_it;
      return victim;
    }
  }

  // TODO: Write this using a switch case statement
  switch (c->repl_policy)
  {
    case 0: // LRU
    // printf("Entered\n");
      // find oldest entry in set
      for(LRU_way_ind = 0; LRU_way_ind < c->num_ways; LRU_way_ind++)
      {
        if(c->sets[set_index].line[LRU_way_ind].valid != 0 && \
           c->sets[set_index].line[LRU_way_ind].last_access_time < earliest_time)
        {
          earliest_ind = LRU_way_ind;
          earliest_time = c->sets[set_index].line[LRU_way_ind].last_access_time;
        }
      }
      victim = earliest_ind;
      return victim;
      break;

    case 1:
      // srand(time(0));
      victim = rand() % c->num_ways;
      return victim;
      break;

    case 2:
      for(it = 0; it < c->num_ways; it++)
      {
        // printf("%d ", c->sets[set_index].line[it].core_id);
        if(c->sets[set_index].line[it].valid != 0 && \
           c->sets[set_index].line[it].core_id == 0)
        {
          occ0++;
          if(oldest0 > c->sets[set_index].line[it].last_access_time)
          {
            oldest0 = c->sets[set_index].line[it].last_access_time;
            old_ind0 = it;
          }
        }
        else if(c->sets[set_index].line[it].valid != 0 && \
                c->sets[set_index].line[it].core_id == 1)
        {
          occ1++;
          if(oldest1 > c->sets[set_index].line[it].last_access_time)
          {
            oldest1 = c->sets[set_index].line[it].last_access_time;
            old_ind1 = it;
          }
        }
      }
      // printf("\n");
      // printf("%d\n",  c->num_ways);
      if(core_id == 0)
      {
        if(occ0 < q0)
        {
          victim = old_ind1;
        }
        else if(occ0 >= q0)
        {
          victim = old_ind0;
        }
      }
      else if(core_id == 1)
      {
        if(occ1 < q1)
        {
          victim = old_ind0;
        }
        else if(occ1 >= q1)
        {
          victim = old_ind1;
        }
      }
      return victim;
      break;

    case 3: // UCP
      for(it = 0; it < c->num_ways; it++)
      {
        // printf("%d ", c->sets[set_index].line[it].core_id);
        if(c->sets[set_index].line[it].valid != 0 && \
           c->sets[set_index].line[it].core_id == 0)
        {
          occ0++;
          if(oldest0 > c->sets[set_index].line[it].last_access_time)
          {
            oldest0 = c->sets[set_index].line[it].last_access_time;
            old_ind0 = it;
          }
        }
        else if(c->sets[set_index].line[it].valid != 0 && \
                c->sets[set_index].line[it].core_id == 1)
        {
          occ1++;
          if(oldest1 > c->sets[set_index].line[it].last_access_time)
          {
            oldest1 = c->sets[set_index].line[it].last_access_time;
            old_ind1 = it;
          }
        }
      }
      // printf("\n");
      // printf("%d\n",  c->num_ways);
      if(core_id == 0)
      {
        if(occ0 < q0)
        {
          victim = old_ind1;
        }
        else if(occ0 >= q0)
        {
          victim = old_ind0;
        }
      }
      else if(core_id == 1)
      {
        if(occ1 < q1)
        {
          victim = old_ind0;
        }
        else if(occ1 >= q1)
        {
          victim = old_ind1;
        }
      }
      return victim;
      break;

    default:
      victim = -1;
      return victim;
  }


  return victim;
}

uns64 partition(UMON **umon)
{
  // printf("\nPartition Called %lld\n", cycle);
  uns64 opt_part = 1;
  uns64 U, U_opt = 1;
  uns i;
  uns N = umon[0]->nways;

  for(i=1; i < N-1; i++)
  {
    U = u_func(umon[0], 1, i) + u_func(umon[1], 1, N-i);
    // printf("UA UB %lld %lld\n", u_func(umon[0], 1, i), u_func(umon[1], 1, N-i));
    if(U > U_opt)
    {
      // exit(-1);
      U_opt = U;
      opt_part = i;
    }
  }
  return opt_part;
}

uns64 u_func(UMON *umon, uns a, uns b)
{
  uns64 utility = 0;
  utility = specmiss(umon, a) - specmiss(umon, b);
  // printf("UTILITYa_b %lld\n", utility);
  return utility;
}

uns64 specmiss(UMON *u, uns i)
{
  uns64 ret = u->misscount;
  uns64 it;
  for(it = i; it < u->nways-1; it++)
  {
    ret += u->ctr[it];
    // printf("CTR %lld\n", u->ctr[0]);
    // if(u->ctr[it] == 2)
    //   exit(1);

  }
  return ret;
}
