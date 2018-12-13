#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "cache.h"



//---- Your answers must be stored in this (top N used for N-way cache)
extern uns conflict_list[8];


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void fill_conflict_list(Cache *c, uns num_ways, uns probe_addr){
  // uns miss;
  // uns cline_addr1=rand()%(1<<20);
  // uns cline_addr2=rand()%(1<<20);
  // uns cline_addr3=rand()%(1<<20);
  // uns cline_addr4=rand()%(1<<20);

  int N = c->num_ways;
  int count = 0;
  int T = 1<<20;
  // install probe address
  long int addr_itr = -1;
  uns ignore_flag = 0;
  uns conflict_flag = 0;

  for(int i = 0; i < 8; i++) conflict_list[i] = -1;

  cache_access_install(c, probe_addr);
  while(true)
  {
    addr_itr = (addr_itr + 1) % T;
    // std::cout << addr_itr << std::endl;
    if(count >= N)
    {
      break;
    }

    ignore_flag = 0;
    // conflict_flag = 0;
    if(addr_itr == probe_addr)
      ignore_flag = 1;
    for(int i = 0; i < count; i++)
    {
      if(addr_itr == conflict_list[i])
        ignore_flag = 1;
    }
    if(ignore_flag != 0)
      continue;

    cache_access_install(c, addr_itr);
    // for(int i = 0; i < count; i++)
    // {
    //   if(cache_access_install(c, conflict_list[i]) == MISS)
    //   {
    //     conflict_flag = 1;
    //     // std::cout << addr_itr << std::endl;
    //     break;
    //   }
    // }
    if(cache_access_install(c, probe_addr) == MISS)
      conflict_flag = 1;
    if(conflict_flag != 0)
    {
      conflict_list[count] = addr_itr;
      count++;
      conflict_flag = 0;
    }
  }

  // for(int i = 0; i < N; i++)
  // {
  //   std::cout << "CONF" << i << " " << conflict_list[i] << std::endl;
  // }

  // //---- The function you should use to determine conflicts
  // // --- The second argument must be less than (1<<20);
  //
  // if( cache_access_install(c, probe_addr)==MISS ){
  //   miss++;
  // }
  //
  // //------- Your code goes above this line
  //
  // conflict_list[0]=cline_addr1; //--- for 1 way
  // conflict_list[1]=cline_addr2; //--- for 2 way
  // conflict_list[2]=cline_addr3; //--- for 4 way
  // conflict_list[3]=cline_addr4; //--- for 4 way
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
