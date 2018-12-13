 /*************************************************************************
 * File         : sim.cc
 * Author       : Moinuddin K. Qureshi
 * Description  : Prime+Probe (Lab 5)  ECE4100/ECE6100
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <sys/time.h>


#include "mtrand.h"

#include "types.h"
#include "cache.h"

#define MAX_CONFLICT_ADDR 8
#define MAX_ITER 100

uns conflict_list[MAX_CONFLICT_ADDR]; //-- this array holds your conflict address

MTRand *randgen=new MTRand(1234);
Cache *mycache;

uns NUM_WAYS=1;
uns PROBE_ADDR=0;

void fill_conflict_list(Cache *c, uns num_ways, uns lineaddr);
void test_conflict_list(Cache *c, uns num_ways, uns lineaddr);

/////////////////////////////////////////////////////////
// Provides elapsed Time between t1 and t2 in milli sec
/////////////////////////////////////////////////////////

double elapsedTime(timeval t1, timeval t2){
  double delta;
  delta = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
  delta += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
  return delta;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  Flag ii;

  if(argc<3){
    printf("./sim <Assoc> <ProbeAddr> (Assoc=1/2/4)(ProbeAddr= 0 to 1 million)\n");
    printf("Exiting ...\n");
    exit(-1);
  }

  NUM_WAYS = atoi(argv[1]);
  PROBE_ADDR = atoi(argv[2]);


  printf("\nStarting test for %u ways and ProbeAddr: %u\n", NUM_WAYS, PROBE_ADDR);

  mycache = cache_new(NUM_WAYS);

  timeval t1, t2;

  // start timer
  gettimeofday(&t1, NULL);

  fill_conflict_list(mycache, NUM_WAYS, PROBE_ADDR);

   // start timer
  gettimeofday(&t2, NULL);

  printf("Search took %6.3f seconds\n", elapsedTime(t1,t2)/1000.0);


  test_conflict_list(mycache, NUM_WAYS, PROBE_ADDR);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void test_conflict_list(Cache *c, uns num_ways, uns probe_addr){
  uns iter, ii, miss=0;
  Flag pass=FALSE;


  for(iter=0; iter< MAX_ITER; iter++){
    for(ii=0; ii< num_ways; ii++){
      cache_access_install(c, conflict_list[ii]);
    }
    if( cache_access_install(c, probe_addr)==MISS ){
      miss++;
    }
  }
  // printf(" MISS %d\n", miss);

  //printf("Miss Rate for ProbeAddr:%x ConfAddr1: %x ConfAddr2: %x is %u\n", probe_addr, conflict_list[0], conflict_list[1], miss);

  if(num_ways==1){
    if(miss > 90){
      pass=TRUE;
    }
  }

  if(num_ways==2){
    if(miss > 60){
      pass=TRUE;
    }
  }

   if(num_ways==4){
    if(miss > 25){
      pass=TRUE;
    }
  }

   if(pass){
     printf("\nOUTCOME: Success\n\n");
   }else{
     printf("\nOUTCOME: Failure\n\n");
   }


}
