#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "cache.h"


extern uns64 cycle; // You can use this as timestamp for LRU

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

void cache_install(Cache *c, Addr lineaddr, uns is_write, uns core_id){

  // Your Code Goes Here
  // Find victim using cache_find_victim
  // Initialize the evicted entry
  // Initialize the victime entry

  // Variables needed
  int32 vic_ind = 0;
  uns set_idx = (lineaddr % c->num_sets) & (c->num_sets-1);
  uns32 tag = (lineaddr) / c->num_sets;

  if(set_idx >= c->num_sets)
  {
    printf("Idx error\n");
    exit(0);
  }

  // find victim
  vic_ind = cache_find_victim(c, set_idx, core_id);
  // Check dirty bit
  if(c->sets[set_idx].line[vic_ind].dirty != 0)
  {
    c->stat_dirty_evicts++;
    // need to write to mem? Happens in bg maybe
  }
  // // Sanity check
  // if(vic_ind >= c->num_ways)
  // {
  //   printf("Vic Ind error %d\n", vic_ind);
  //   printf("Check full %d\n", check_full);
  //   cache_print_stats(c, "L1");
  //   exit(0);
  // }

  // Update last evicted line
  c->last_evicted_line = c->sets[set_idx].line[vic_ind];

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

  for(set_it = 0; set_it < c->num_ways; set_it++)
  {
    if(c->sets[set_index].line[set_it].valid == 0)
    {
      victim=set_it;
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

    default:
      victim = -1;
      return victim;
  }


  return victim;
}
