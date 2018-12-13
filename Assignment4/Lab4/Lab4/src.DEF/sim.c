 /*************************************************************************
 * File         : sim.c
 * Author       : Moinuddin K. Qureshi 
 * Description  : CMP Memory System Lab 4 of ECE4100/ECE6100
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include "types.h"
#include "memsys.h"
#include "core.h"

#define PRINT_DOTS   1
#define DOT_INTERVAL 100000

/***************************************************************************
 * Globals 
 **************************************************************************/

MODE        SIM_MODE        = SIM_MODE_A;
uns64       CACHE_LINESIZE  = 64;
uns64       REPL_POLICY     = 0; // 0:LRU 1:RAND

uns64       DCACHE_SIZE     = 32*1024; 
uns64       DCACHE_ASSOC    = 8; 

uns64       ICACHE_SIZE     = 32*1024; 
uns64       ICACHE_ASSOC    = 8; 

uns64       L2CACHE_SIZE    = 1024*1024; 
uns64       L2CACHE_ASSOC   = 16;
uns64       L2CACHE_REPL    = 0; // 0:LRU 1:RND 2:SWP 3:UCP

uns64       SWP_CORE0_WAYS  = 0;

uns64       NUM_CORES       = 1;


/***************************************************************************************
 * Functions
 ***************************************************************************************/
void print_dots(void);
void die_usage();
void die_message(const char * msg);
void get_params(int argc, char** argv);
void print_stats();

/***************************************************************************************
 * Globals
 ***************************************************************************************/

Memsys      *memsys;
Core        *core[MAX_CORES];
char        trace_filename[MAX_CORES][1024];
uns64       last_printdot_cycle;
uns64       cycle;

/***************************************************************************************
 * Main
 ***************************************************************************************/
int main(int argc, char** argv)
{
  Flag ii;

    srand(42);

    get_params(argc, argv);

    assert(NUM_CORES<=MAX_CORES);

    //---- Initiliaze the system
    memsys = memsys_new();

    for(ii=0; ii<NUM_CORES; ii++){
	core[ii] = core_new(memsys,trace_filename[ii], ii);
    }

    print_dots();

    //--------------------------------------------------------------------
    // -- Iterate until all cores are done
    //--------------------------------------------------------------------
    Flag all_cores_done=0;
    
    while( ! (all_cores_done) ){
       all_cores_done=1;
      
      for(ii=0; ii<NUM_CORES; ii++){
	core_cycle(core[ii]);
	all_cores_done &= core[ii]->done;
      }
      
      if (cycle - last_printdot_cycle >= DOT_INTERVAL){
	print_dots();
      }
      
      cycle++; 
    }
    
    print_stats();
    return 0;
}

//--------------------------------------------------------------------
// -- Print statistics
//--------------------------------------------------------------------

void print_stats(){
  uns ii;

  printf("\n");
  printf("\nCYCLES      \t\t\t : %10llu", cycle);
  
  for(ii=0; ii<NUM_CORES; ii++){
    core_print_stats(core[ii]);
  }
  
  memsys_print_stats(memsys);
  
  printf("\n\n");
}

//--------------------------------------------------------------------
// -- Print Hearbeats 
//--------------------------------------------------------------------


void print_dots(){
  uns LINE_INTERVAL = 50 *  DOT_INTERVAL;

  last_printdot_cycle = cycle;

  if(!PRINT_DOTS){
      return;
  }

  if (cycle % LINE_INTERVAL ==0){
	printf("\n%4llu M\t", cycle/1000000);
	fflush(stdout);
    }
    else{
	printf(".");
	fflush(stdout);
    }
    
}


//--------------------------------------------------------------------
// -- Usage Menu
//--------------------------------------------------------------------

void die_usage() {
    printf("Usage : sim [-option <value>] trace_0 <trace_1> \n");
    printf("   Options\n");
    printf("      -mode            <num>    Set mode of the simulator[1:PartA, 2:PartB, 3:PartC 4:PartD 5:PartE]  (Default: 1)\n");
    printf("      -linesize        <num>    Set cache linesize for all caches (Default:64)\n");
    printf("      -repl            <num>    Set replacement policy for L1 cache [0:LRU,1:RND] (Default:0)\n");
    printf("      -DsizeKB         <num>    Set capacity in KB of the the Level 1 DCACHE (Default:32 KB)\n");
    printf("      -Dassoc          <num>    Set associativity of the the Level 1 DCACHE (Default:8)\n");
    printf("      -L2sizeKB        <num>    Set capacity in KB of the unified Level 2 cache (Default: 512 KB)\n");
    printf("      -L2repl          <num>    Set replacement policy for L2 cache [0:LRU,1:RND,2:SWP] (Default:0)\n");
    printf("      -SWP_core0ways   <num>    Set static quota for core_0 for SWP (Default:1)\n");
    exit(0);
}

//--------------------------------------------------------------------
// -- Print Error Message and Die
//--------------------------------------------------------------------

												          
void die_message(const char * msg) {
    printf("Error! %s. Exiting...\n", msg);
    exit(1);
}

//--------------------------------------------------------------------
// -- Read Parameters from Command Line
//--------------------------------------------------------------------

void get_params(int argc, char** argv){
  int   ii;
  int  num_trace_filename=0;

  if (argc < 2) {
    die_usage();
  }

    //--------------------------------------------------------------------
    // -- Get command line options
    //--------------------------------------------------------------------    
    for ( ii = 1; ii < argc; ii++) {
	if (argv[ii][0] == '-') {	    
	    if (!strcmp(argv[ii], "-h") || !strcmp(argv[ii], "-help")) {
		die_usage();
	    }	    

	    else if (!strcmp(argv[ii], "-mode")) {
		if (ii < argc - 1) {		  
     		  SIM_MODE = atoi(argv[ii+1]);
		  ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-linesize")) {
		if (ii < argc - 1) {		  
		    CACHE_LINESIZE = atoi(argv[ii+1]);
		    ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-repl")) {
		if (ii < argc - 1) {		  
		    REPL_POLICY = atoi(argv[ii+1]);
		    ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-DsizeKB")) {
		if (ii < argc - 1) {		  
		    DCACHE_SIZE = atoi(argv[ii+1])*1024;
		    ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-Dassoc")) {
		if (ii < argc - 1) {		  
		    DCACHE_ASSOC = atoi(argv[ii+1]);
		    ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-L2sizeKB")) {
		if (ii < argc - 1) {		  
		    L2CACHE_SIZE = atoi(argv[ii+1])*1024;
		    ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-L2repl")) {
		if (ii < argc - 1) {		  
		    L2CACHE_REPL = atoi(argv[ii+1]);
		    ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-SWP_core0ways")) {
		if (ii < argc - 1) {		  
		    SWP_CORE0_WAYS = atoi(argv[ii+1]);
		    ii += 1;
		}
	    }
	    
	    else {
		char msg[256];
		sprintf(msg, "Invalid option %s", argv[ii]);
		die_message(msg);
	    }
	}
	else if (num_trace_filename<MAX_CORES) {
	    strcpy(trace_filename[num_trace_filename], argv[ii]);
	    num_trace_filename++;
	    NUM_CORES=num_trace_filename;
	}
	else {
	    char msg[256];
	    sprintf(msg, "Invalid option %s, got filename %s", argv[ii], trace_filename[NUM_CORES]);
	    die_message(msg);
	}    
    }
	    
    //--------------------------------------------------------------------
    // Error checking
    //--------------------------------------------------------------------
    if (num_trace_filename==0) {
	die_message("Must provide at least one trace file");
    }


  
}
