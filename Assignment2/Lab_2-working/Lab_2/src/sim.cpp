/********************************************************************
 * File         : sim.cpp
 * Description  :  Pipeline for Lab2 ECE 6100
 *********************************************************************/

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pipeline.h"

#define HEARTBEAT_CYCLES 5


/*********************************************************************
 * Global Scope Functions
 *********************************************************************/

void die_message(const char *msg) {
    printf("Error! %s. Exiting...\n", msg);
    exit(1);
}

void die_usage() {
    printf("Usage : sim [options] <trace_file> \n\n");
    printf("Trace driven pipeline simulator\n");
    printf("Options\n");
    printf("   -pipewidth   <num>    Set width of pipeline to <num> (Default: 1)\n");
    printf("   -enablememfwd         Enable forwarding from MEM stage (Default: off)\n");
    printf("   -enableexefwd         Enable forwarding from EXE stage (Default: off)\n");
    printf("   -bpredpolicy <num>    Set branch predictor  [0:Perf 1:Taken 2:Gshare]\n");
}

void check_heartbeat(void);

void print_stats(void);


/*********************************************************************
 * Params and Globals
 *********************************************************************/
uint32_t  PIPE_WIDTH=1;
uint32_t  ENABLE_MEM_FWD=0;
uint32_t  ENABLE_EXE_FWD=0;
uint32_t  BPRED_POLICY=0; // 0:Perf 1:AlwaysTaken 2:Gshare

Pipeline *pipeline;
/*********************************************************************
 * Main
 *********************************************************************/

int main(int argc, char *argv[])
{
  int ii;

    FILE *tr_file;
    char tr_filename[1024];
    char cmd_string[256];

    if(argc < 1) {
        die_message("Must Provide a Trace File");
    }

    //--------------------------------------------------------------------
    // -- Get params from command line
    //--------------------------------------------------------------------
    for ( ii = 1; ii < argc; ii++) {
	if (argv[ii][0] == '-') {
	    if (!strcmp(argv[ii], "-h") || !strcmp(argv[ii], "-help")) {
		die_usage();
	    }


	    else if (!strcmp(argv[ii], "-pipewidth")) {
		if (ii < argc - 1) {
		    PIPE_WIDTH = atoi(argv[ii+1]);
		    ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-bpredpolicy")) {
		if (ii < argc - 1) {
		    BPRED_POLICY = atoi(argv[ii+1]);
		    ii += 1;
		}
	    }

	    else if (!strcmp(argv[ii], "-enablememfwd")) {
	      ENABLE_MEM_FWD = 1;
	    }

	    else if (!strcmp(argv[ii], "-enableexefwd")) {
	      ENABLE_EXE_FWD = 1;
	    }
	}
	else {
	  strcpy(tr_filename, argv[ii]);
	}
    }


  // ------- Open Trace File -------------------------------------------
    sprintf(cmd_string,"gunzip -c %s", tr_filename);
    if ((tr_file = popen(cmd_string, "r")) == NULL){
        printf("Command string is %s\n", cmd_string);
        die_message("Unable to open the trace file with gzip option \n")  ;
    } else {
        printf("Opened file with command: %s \n", cmd_string);
    }

  // ------- Pipeline Initialization & Execution ----------------------

     pipeline = pipe_init(tr_file);
    while(!pipeline->halt) {
      pipe_cycle(pipeline);
      check_heartbeat();
    }

  // ------- Print Statistics------------------------------------------
    print_stats();
    fclose(tr_file);
    return 0;
}

/*********************************************************************
 * Print Statistics
 *********************************************************************/

void print_stats(void) {
    char header[256];
    sprintf(header, "LAB2");
    uint64_t stat_num_inst       = pipeline->stat_retired_inst;
    uint64_t stat_num_cycle      = pipeline->stat_num_cycle;
    double cpi = (double)(stat_num_cycle)/(double)(stat_num_inst);

    printf("\n\n");

    printf("\n%s_NUM_INST           \t : %10u" , header, (uint32_t)stat_num_inst)  ;
    printf("\n%s_NUM_CYCLES         \t : %10u" , header, (uint32_t)stat_num_cycle);
    printf("\n%s_CPI                \t : %10.3f" , header, cpi);

    if(BPRED_POLICY){
    printf("\n%s_BPRED_BRANCHES     \t : %10u" , header, (uint32_t)pipeline->b_pred->stat_num_branches)  ;
    printf("\n%s_BPRED_MISPRED      \t : %10u" , header, (uint32_t)pipeline->b_pred->stat_num_mispred)  ;
    printf("\n%s_MISPRED_RATE       \t : %10.3f" , header, 100.0*(double)(pipeline->b_pred->stat_num_mispred)/(double)(pipeline->b_pred->stat_num_branches));
    }

    printf("\n\n");
}

/*********************************************************************
 * Print Heartbeat
 *********************************************************************/
uint64_t last_hbeat_cycle;
uint64_t last_hbeat_line;
uint64_t last_hbeat_inst;

void check_heartbeat(void){

  if(pipeline->stat_num_cycle - last_hbeat_cycle < HEARTBEAT_CYCLES){
    return;
  }

  printf(".");
  fflush(stdout);

  // check for deadlock
  if(last_hbeat_inst == pipeline->stat_retired_inst){
    printf("No committed instructions in %u cycles.\n", HEARTBEAT_CYCLES);
    die_message("Pipeline is Deadlocked. Dying\n");
  }

  last_hbeat_cycle=pipeline->stat_num_cycle;
  last_hbeat_inst = pipeline->stat_retired_inst;

  // print a newline and CPI every so often
  if(pipeline->stat_num_cycle - last_hbeat_line >= 50*HEARTBEAT_CYCLES){
    printf("\n(Inst:%8u\tCycle:%8u\tCPI:%6.3f)\t", (uint32_t)pipeline->stat_retired_inst,
	   (uint32_t)pipeline->stat_num_cycle, (double)(pipeline->stat_num_cycle)/(double)(pipeline->stat_retired_inst+1));
    last_hbeat_line=pipeline->stat_num_cycle;
  }
}
