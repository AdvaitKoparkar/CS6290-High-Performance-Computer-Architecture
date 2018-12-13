#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mtrand.h"

#include "cache.h"

#define NUM_SETS 1024


extern MTRand* randgen;

////////////////////////////////////////////////////////////////////
// ------------- DO NOT MODIFY THIS FILE -----------
////////////////////////////////////////////////////////////////////

Cache  *cache_new(uns assoc){

   Cache *c = (Cache *) calloc (1, sizeof (Cache));
   c->num_ways = assoc;

   // determine num sets, and init the cache
   c->num_sets = NUM_SETS; 
   c->sets  = (Cache_Set *) calloc (c->num_sets, sizeof(Cache_Set));

   cache_reset(c);
   return c;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void cache_reset(Cache *c){
  uns ii, setid;

  for(setid=0; setid<NUM_SETS; setid++){
    for(ii=0; ii< c->num_ways; ii++){
      c->sets[setid].line[ii].valid=FALSE;
    }
  }

  // initalize TBR
  for(ii=0; ii< TBR_ENTRIES; ii++){
    c->tbr[ii]=ii;
  }

  
  for(ii=0; ii< TBR_ENTRIES; ii++){
    uns dest = randgen->randInt()%(TBR_ENTRIES);
    uns tmp = c->tbr[dest];
    c->tbr[dest] = c->tbr[ii];
    c->tbr[ii] = tmp;
  }
 
   
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

Flag cache_access_install(Cache *c, uns lineaddr) {

    assert(lineaddr < TBR_ENTRIES);

    uns elineaddr = c->tbr[lineaddr];
    uns index = elineaddr % c->num_sets; 

    for (uns ii = 0; ii < c->num_ways; ii++) {
      if( c->sets[index].line[ii].valid && (c->sets[index].line[ii].tag == lineaddr)){
	return HIT;
      }
    }

    //--- install on a miss (Random replacement for simplicity)

    uns victim = randgen->randInt()%c->num_ways;
    c->sets[index].line[victim].valid =TRUE;
    c->sets[index].line[victim].tag = lineaddr;

    return MISS;
}
