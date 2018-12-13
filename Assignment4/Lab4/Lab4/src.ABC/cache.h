#ifndef CACHE_H
#define CACHE_H

#include "types.h"

#define MAX_WAYS 16

typedef struct Cache_Line Cache_Line;
typedef struct Cache_Set Cache_Set;
typedef struct Cache Cache;

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


struct Cache_Line {
    Flag    valid;
    Flag    dirty;
    Addr    tag;
    uns     core_id;
    uns    last_access_time; // for LRU
   // Note: No data as we are only estimating hit/miss 
};


struct Cache_Set {
    Cache_Line line[MAX_WAYS];
};


struct Cache{
  uns64 num_sets;
  uns64 num_ways;
  uns64 repl_policy;
  
  Cache_Set *sets;
  Cache_Line last_evicted_line; // for checking writebacks

  //stats
  uns64 stat_read_access; 
  uns64 stat_write_access; 
  uns64 stat_read_miss; 
  uns64 stat_write_miss; 
  uns64 stat_dirty_evicts; // how many dirty lines were evicted?
};


/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

Cache  *cache_new(uns64 size, uns64 assocs, uns64 linesize, uns64 repl_policy);
Flag    cache_access         (Cache *c, Addr lineaddr, uns is_write, uns core_id);
void    cache_install        (Cache *c, Addr lineaddr, uns is_write, uns core_id);
void    cache_print_stats    (Cache *c, char *header);

uns     cache_find_victim    (Cache *c, uns set_index, uns core_id);

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#endif // CACHE_H
