/***********************************************************************
 * File         : pipeline.cpp
 * Author       : Soham J. Desai
 * Date         : 14th January 2014
 * Description  : Superscalar Pipeline for Lab2 ECE 6100
 **********************************************************************/

#include "pipeline.h"
#include <cstdlib>

extern int32_t PIPE_WIDTH;
extern int32_t ENABLE_MEM_FWD;
extern int32_t ENABLE_EXE_FWD;
extern int32_t BPRED_POLICY;

// my code start
bool show_clash = false;
int SAME_ID_DEP;
// int WAW_STALL;
// long long int stalled = 0;
// long long int unstalled = 0;

/**********************************************************************
 * Support Function: Read 1 Trace Record From File and populate Fetch Op
 **********************************************************************/

void pipe_get_fetch_op(Pipeline *p, Pipeline_Latch* fetch_op){
    uint8_t bytes_read = 0;
    bytes_read = fread(&fetch_op->tr_entry, 1, sizeof(Trace_Rec), p->tr_file);

    // check for end of trace
    if( bytes_read < sizeof(Trace_Rec)) {
      fetch_op->valid=false;
      p->halt_op_id=p->op_id_tracker;
      return;
    }

    // got an instruction ... hooray!
    fetch_op->valid=true;
    fetch_op->stall=false;
    fetch_op->is_mispred_cbr=false;
    p->op_id_tracker++;
    fetch_op->op_id=p->op_id_tracker;

    return;
}


/**********************************************************************
 * Pipeline Class Member Functions
 **********************************************************************/

Pipeline * pipe_init(FILE *tr_file_in){
    printf("\n** PIPELINE IS %d WIDE **\n\n", PIPE_WIDTH);

    // Initialize Pipeline Internals
    Pipeline *p = (Pipeline *) calloc (1, sizeof (Pipeline));

    p->tr_file = tr_file_in;
    p->halt_op_id = ((uint64_t)-1) - 3;

    // Allocated Branch Predictor
    if(BPRED_POLICY){
      p->b_pred = new BPRED(BPRED_POLICY);
    }

    return p;
}


/**********************************************************************
 * Print the pipeline state (useful for debugging)
 **********************************************************************/

void pipe_print_state(Pipeline *p){
    std::cout << "--------------------------------------------" << std::endl;
    std::cout <<"cycle count : " << p->stat_num_cycle << " retired_instruction : " << p->stat_retired_inst << std::endl;

    uint8_t latch_type_i = 0;   // Iterates over Latch Types
    uint8_t width_i      = 0;   // Iterates over Pipeline Width
    for(latch_type_i = 0; latch_type_i < NUM_LATCH_TYPES; latch_type_i++) {
        switch(latch_type_i) {
            case 0:
                printf(" FE: ");
                break;
            case 1:
                printf(" ID: ");
                break;
            case 2:
                printf(" EX: ");
                break;
            case 3:
                printf(" MEM: ");
                break;
            default:
                printf(" ---- ");
        }
    }
    printf("\n");
    for(width_i = 0; width_i < PIPE_WIDTH; width_i++) {
        for(latch_type_i = 0; latch_type_i < NUM_LATCH_TYPES; latch_type_i++) {
            if(p->pipe_latch[latch_type_i][width_i].valid == true) {
	      printf(" %6u ",(uint32_t)( p->pipe_latch[latch_type_i][width_i].op_id));
            } else {
                printf(" ------ ");
            }
        }
        printf("\n");
    }
    printf("\n");

}


/**********************************************************************
 * Pipeline Main Function: Every cycle, cycle the stage
 **********************************************************************/

void pipe_cycle(Pipeline *p)
{
    p->stat_num_cycle++;

    // print_pipeline_condition(p);
    pipe_cycle_WB(p);
    pipe_cycle_MEM(p);
    pipe_cycle_EX(p);
    pipe_cycle_ID(p);
    pipe_cycle_FE(p);

}
/**********************************************************************
 * -----------  DO NOT MODIFY THE CODE ABOVE THIS LINE ----------------
 **********************************************************************/

void pipe_cycle_WB(Pipeline *p){
  int ii;

  if(p->stat_num_cycle == 1)
  {

    for(ii=0; ii<PIPE_WIDTH; ii++)
    {

      // my code start
      // if this is the first instruction cycle
      // then
      //   make all latches invalid
      //   make all latches unstalled

      {
        // std::cout << "TEST";
        for(int j = 0; j < NUM_LATCH_TYPES; j++)
        {
          p->pipe_latch[j][ii].valid = false;
          p->pipe_latch[j][ii].stall = false;
        }
      }
    }
  }

  for(int ii=0; ii < PIPE_WIDTH; ii++)
  {
    if(p->pipe_latch[MEM_LATCH][ii].valid)
    {
      p->stat_retired_inst++;
      if(p->pipe_latch[MEM_LATCH][ii].op_id >= p->halt_op_id)
      {
	       p->halt=true;
         // // my code start
         // std::cout << "STALLED: " << stalled << "\n";
         // std::cout << "UNSTALLED: " << unstalled << "\n";
         // // my code end
      }
      // if the data from mem_latch comes to WB
      // reset src1, src2, and dest needed vars
      p->pipe_latch[MEM_LATCH][ii].valid = false;
      p->pipe_latch[MEM_LATCH][ii].tr_entry.dest_needed = 0;
      p->pipe_latch[MEM_LATCH][ii].tr_entry.src1_needed = 0;
      p->pipe_latch[MEM_LATCH][ii].tr_entry.src2_needed = 0;
      p->pipe_latch[MEM_LATCH][ii].tr_entry.cc_write = 0;
      // if(verbose)
      // {
      //   std::cout << "STAGE: WB | INST COMPLETED: " << p->stat_retired_inst << " | PIPE #" << ii << ": RECEIVED INST\n";
      // }
      // my code end
    }

  }
}

//--------------------------------------------------------------------//

void pipe_cycle_MEM(Pipeline *p){
  int ii;
  for(ii=0; ii<PIPE_WIDTH; ii++)
  {
    // my code start

    // if MEM_LATCH is invalid AND
    // if EXE_LATCH is valid
    // then
    //   fetch new instruction from EXE
    //   make MEM_LATCH valid
    //   make EX_LATCH invalid
    if(p->pipe_latch[MEM_LATCH][ii].valid == false && \
       p->pipe_latch[EX_LATCH][ii].valid == true)
    {
      p->pipe_latch[MEM_LATCH][ii]=p->pipe_latch[EX_LATCH][ii];
      p->pipe_latch[MEM_LATCH][ii].valid = true;
      p->pipe_latch[EX_LATCH][ii].valid = false;

      // if(verbose)
      // {
      //   std::cout << "STAGE: MEM | INST COMPLETED: " << p->stat_retired_inst << " | PIPE #" << ii << ": RECEIVED INST\n";
      // }
    }
    // my code end
  }
}

//--------------------------------------------------------------------//

void pipe_cycle_EX(Pipeline *p){
  int ii;
  for(ii=0; ii<PIPE_WIDTH; ii++)
  {
    // my code start
    // if ID_LATCH is valid AND
    // if ID_LATCH is not stalled AND
    // if EXE_LATCH is invalid
    // then
    //  get instruction in the EXE latch
    //  make ID_LATCH invalid
    //  make EX_LATCH valid

    // ---------------------------------
    // else if EX_LATCH is invalid AND
    //      if MEM_LATCH is invalid AND
    //      if ID_LATCH is valid AND
    //      if ID_LATCH is stalled (which means pipeline has been cleared)
    // then
    //  Get data into EX from ID
    //  remove STALL in ID
    //  remove stall in FE
    //  make ID invalid
    // else
    //  make EX_LATCH invalid
    // -----------------------------------

    if(p->pipe_latch[ID_LATCH][ii].stall == false && \
       p->pipe_latch[ID_LATCH][ii].valid == true && \
       p->pipe_latch[EX_LATCH][ii].valid == false)
    {
      p->pipe_latch[EX_LATCH][ii]=p->pipe_latch[ID_LATCH][ii];
      p->pipe_latch[ID_LATCH][ii].valid = false;
      p->pipe_latch[EX_LATCH][ii].valid = true;
    }
    // else if(p->pipe_latch[EX_LATCH][ii].valid == false && \
    //         p->pipe_latch[MEM_LATCH][ii].valid == false && \
    //         p->pipe_latch[ID_LATCH][ii].valid == true && \
    //         p->pipe_latch[ID_LATCH][ii].stall == true)
    // {
    //   p->pipe_latch[ID_LATCH][ii].stall = false;
    //   p->pipe_latch[FE_LATCH][ii].stall = false;
    //   p->pipe_latch[EX_LATCH][ii]=p->pipe_latch[ID_LATCH][ii];
    //   p->pipe_latch[ID_LATCH][ii].valid = false;
    // }

    // else
    // {
    //   p->pipe_latch[EX_LATCH][ii].valid = false;
    // }
    // my code end
  }
}

//--------------------------------------------------------------------//

void pipe_cycle_ID(Pipeline *p){

  // my code start
  uint64_t OLDEST_STALL_OP_ID = p->halt_op_id + 1;
  uint64_t NEWEST_EXE_LD_FWD;
  uint64_t NEWEST_EXE_LD_CC;
  uint64_t NEWEST_EXE_CC;
  uint64_t NEWEST_EXE_CC_idx;
  bool CHECK_STAGE;
  bool EXE_STALL_CC_LD_DEC_PENDING;



  for(int ii = 0; ii < PIPE_WIDTH; ii++)
  {

    if(p->pipe_latch[FE_LATCH][ii].valid == true) // always true - safety net
    {
      // clear all stalls in ID and FE
      p->pipe_latch[ID_LATCH][ii].stall = false;
      p->pipe_latch[FE_LATCH][ii].stall = false;
    }
  }

  for(int ii = 0; ii < PIPE_WIDTH; ii++)
  {
    CHECK_STAGE = true;
    for(int pw_i = 0; pw_i < PIPE_WIDTH && CHECK_STAGE; pw_i++)
    {
        // update stall for pw_i pipeline
      if(PIPE_WIDTH > 1 && ii != pw_i && \
              p->pipe_latch[FE_LATCH][pw_i].op_id < p->pipe_latch[FE_LATCH][ii].op_id && \
              p->pipe_latch[FE_LATCH][pw_i].valid && \
              p->pipe_latch[FE_LATCH][ii].valid && \
              p->pipe_latch[FE_LATCH][pw_i].tr_entry.dest_needed && \
              p->pipe_latch[FE_LATCH][ii].tr_entry.src1_needed && \
              p->pipe_latch[FE_LATCH][ii].tr_entry.src1_reg == \
              p->pipe_latch[FE_LATCH][pw_i].tr_entry.dest)
      {
        if(show_clash)
        {
          std::cout << "SRC1_CLASH_MEM_ID" << " PP_SRC1_ID\n";
          std::cout << "ID Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "\
                    << p->pipe_latch[FE_LATCH][pw_i].op_id << "\n";
        }
        // stall
        CHECK_STAGE = false;
        p->pipe_latch[FE_LATCH][ii].stall = true;
        // break;
      }
      else if(PIPE_WIDTH > 1 && ii != pw_i && \
              p->pipe_latch[FE_LATCH][pw_i].op_id < p->pipe_latch[FE_LATCH][ii].op_id && \
              p->pipe_latch[FE_LATCH][pw_i].valid && \
              p->pipe_latch[FE_LATCH][ii].valid && \
              p->pipe_latch[FE_LATCH][pw_i].tr_entry.dest_needed && \
              p->pipe_latch[FE_LATCH][ii].tr_entry.src2_needed && \
              p->pipe_latch[FE_LATCH][ii].tr_entry.src2_reg == p->pipe_latch[FE_LATCH][pw_i].tr_entry.dest)
      {
        if(show_clash)
        {
          std::cout << "SRC2_CLASH_MEM_ID" << " PP_SRC2_ID\n";
          std::cout << "ID Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "\
                    << p->pipe_latch[FE_LATCH][pw_i].op_id << "\n";
        }
        // stall
        CHECK_STAGE = false;
        p->pipe_latch[FE_LATCH][ii].stall = true;
        // break;
      }
      else if(PIPE_WIDTH > 1 && ii != pw_i && \
              p->pipe_latch[FE_LATCH][pw_i].op_id < p->pipe_latch[FE_LATCH][ii].op_id && \
              p->pipe_latch[FE_LATCH][pw_i].valid && \
              p->pipe_latch[FE_LATCH][ii].valid && \
              int(p->pipe_latch[FE_LATCH][pw_i].tr_entry.cc_write) != 0 && \
              int(p->pipe_latch[FE_LATCH][ii].tr_entry.cc_read) != 0)
      {
        if(show_clash)
        {
          std::cout << "SRC1_CLASH_MEM_ID" << " PP_CC_ID\n";
          std::cout << "ID Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "\
                    << p->pipe_latch[FE_LATCH][pw_i].op_id << "\n";
        }
        // stall
        CHECK_STAGE = false;
        p->pipe_latch[FE_LATCH][ii].stall = true;
        // break;
      }
    }


    EXE_STALL_CC_LD_DEC_PENDING = false;
    NEWEST_EXE_LD_CC = 0;
    NEWEST_EXE_LD_FWD = 0;
    NEWEST_EXE_CC = 0;
    NEWEST_EXE_CC_idx = -1;
    for(int pw_i = 0; pw_i < PIPE_WIDTH && CHECK_STAGE; pw_i++)
    {
      if(p->pipe_latch[EX_LATCH][pw_i].op_id > NEWEST_EXE_LD_FWD && \
         int(p->pipe_latch[EX_LATCH][pw_i].tr_entry.cc_write) != 0 && \
         p->pipe_latch[EX_LATCH][pw_i].op_id == OP_LD)
      {
        // std::cout << "HAHAHAH\n";
        NEWEST_EXE_LD_FWD = p->pipe_latch[EX_LATCH][pw_i].op_id;
      }

      if(p->pipe_latch[EX_LATCH][pw_i].op_id > NEWEST_EXE_LD_FWD && \
         int(p->pipe_latch[EX_LATCH][pw_i].tr_entry.cc_write) != 0)
      {
        NEWEST_EXE_CC = p->pipe_latch[EX_LATCH][pw_i].op_id;
        NEWEST_EXE_CC_idx = pw_i;
      }

      if(p->pipe_latch[FE_LATCH][ii].valid && \
              p->pipe_latch[EX_LATCH][pw_i].valid && \
              int(p->pipe_latch[EX_LATCH][pw_i].tr_entry.cc_write) != 0 && \
              int(p->pipe_latch[FE_LATCH][ii].tr_entry.cc_read) != 0 && \
              p->pipe_latch[EX_LATCH][pw_i].op_id < int(p->pipe_latch[FE_LATCH][ii].op_id))
      {
        if(show_clash)
        {
          std::cout << "CC_CLASH_EX_ID" << " CC\n";
          std::cout << "Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "  \
                  <<  p->pipe_latch[EX_LATCH][pw_i].op_id << "\n";
          std::cout << "OP_TYPE OF " << p->pipe_latch[EX_LATCH][pw_i].op_id << ": " \
                    << int(p->pipe_latch[EX_LATCH][pw_i].tr_entry.op_type) << "\n";
        }
        // stall
        p->pipe_latch[FE_LATCH][ii].stall = true;
        if(ENABLE_EXE_FWD && \
           p->pipe_latch[EX_LATCH][pw_i].tr_entry.op_type != OP_LD)
        {

          p->pipe_latch[FE_LATCH][ii].stall = false;
        }
        else if(ENABLE_EXE_FWD && \
                p->pipe_latch[EX_LATCH][pw_i].tr_entry.op_type == OP_LD)
        {
          // STALL DECISION PENDING
          p->pipe_latch[FE_LATCH][pw_i].stall = false;
          EXE_STALL_CC_LD_DEC_PENDING = true;
          if(NEWEST_EXE_LD_FWD < (p->pipe_latch[EX_LATCH][pw_i].op_id))
          {
            NEWEST_EXE_LD_FWD = (p->pipe_latch[EX_LATCH][pw_i].op_id);
          }
        }
        if(p->pipe_latch[FE_LATCH][ii].stall == true)
        {
          CHECK_STAGE = false;
        }
        // break;
      }
      else if(p->pipe_latch[FE_LATCH][ii].valid && \
         p->pipe_latch[EX_LATCH][pw_i].valid && \
         p->pipe_latch[EX_LATCH][pw_i].tr_entry.dest_needed && \
         p->pipe_latch[FE_LATCH][ii].tr_entry.src1_needed && \
         p->pipe_latch[EX_LATCH][pw_i].tr_entry.dest == \
         p->pipe_latch[FE_LATCH][ii].tr_entry.src1_reg && \
         p->pipe_latch[EX_LATCH][pw_i].op_id < p->pipe_latch[FE_LATCH][ii].op_id)
      {
        if(show_clash)
        {
          std::cout << "SRC1_CLASH_EX_ID"  << " ID1\n";
          std::cout << "Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "\
                    << p->pipe_latch[EX_LATCH][pw_i].op_id << "\n";
        }
        // stall
        p->pipe_latch[FE_LATCH][ii].stall = true;
        if(ENABLE_EXE_FWD && \
           p->pipe_latch[EX_LATCH][pw_i].tr_entry.op_type != OP_LD)
        {
          p->pipe_latch[FE_LATCH][ii].stall = false;
        }
        if(p->pipe_latch[FE_LATCH][ii].stall == true)
        {
          CHECK_STAGE = false;
        }
        // break;
      }
      else if(p->pipe_latch[FE_LATCH][ii].valid && \
             p->pipe_latch[EX_LATCH][pw_i].valid && \
             p->pipe_latch[EX_LATCH][pw_i].tr_entry.dest_needed && \
             p->pipe_latch[FE_LATCH][ii].tr_entry.src2_needed && \
             p->pipe_latch[EX_LATCH][pw_i].tr_entry.dest == \
             p->pipe_latch[FE_LATCH][ii].tr_entry.src2_reg && \
             p->pipe_latch[EX_LATCH][pw_i].op_id < p->pipe_latch[FE_LATCH][ii].op_id)
      {
        if(show_clash)
        {
          std::cout << "SRC2_CLASH_EX_ID" << " ID2\n";
          std::cout << "Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "\
                    << p->pipe_latch[EX_LATCH][pw_i].op_id << "\n";
        }
        // stall
        p->pipe_latch[FE_LATCH][ii].stall = true;
        if(ENABLE_EXE_FWD && \
           p->pipe_latch[EX_LATCH][pw_i].tr_entry.op_type != OP_LD)
        {
          p->pipe_latch[FE_LATCH][ii].stall = false;
        }
        if(p->pipe_latch[FE_LATCH][ii].stall == true)
        {
          CHECK_STAGE = false;
        }
        // break;
      }
    }
    if((EXE_STALL_CC_LD_DEC_PENDING) && \
      p->pipe_latch[EX_LATCH][NEWEST_EXE_CC_idx].tr_entry.op_type == OP_LD)
    {
      p->pipe_latch[FE_LATCH][ii].stall = true;
      CHECK_STAGE = false;
      EXE_STALL_CC_LD_DEC_PENDING = false;
    }



  for(int pw_i = 0; pw_i < PIPE_WIDTH && CHECK_STAGE; pw_i++)
  {
      if(p->pipe_latch[FE_LATCH][ii].valid && \
         (true)  && \
         p->pipe_latch[MEM_LATCH][pw_i].valid && \
         p->pipe_latch[MEM_LATCH][pw_i].tr_entry.dest_needed && \
         p->pipe_latch[FE_LATCH][ii].tr_entry.src1_needed && \
         p->pipe_latch[MEM_LATCH][pw_i].tr_entry.dest == \
         p->pipe_latch[FE_LATCH][ii].tr_entry.src1_reg && \
         p->pipe_latch[MEM_LATCH][pw_i].op_id < p->pipe_latch[FE_LATCH][ii].op_id)
      {
        if(show_clash)
        {
          std::cout << "SRC1_CLASH_MEM_ID" << " ID3\n";
          std::cout << "Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "\
                    << p->pipe_latch[MEM_LATCH][pw_i].op_id << "\n";
        }
        // stall
        p->pipe_latch[FE_LATCH][ii].stall = true;

        if(ENABLE_MEM_FWD)
        {
          p->pipe_latch[FE_LATCH][ii].stall = false;
        }
        // break;
      }
      else if(p->pipe_latch[FE_LATCH][ii].valid && \
        (true) && \
         p->pipe_latch[MEM_LATCH][pw_i].valid && \
         p->pipe_latch[MEM_LATCH][pw_i].tr_entry.dest_needed && \
         p->pipe_latch[FE_LATCH][ii].tr_entry.src2_needed && \
         p->pipe_latch[MEM_LATCH][pw_i].tr_entry.dest == \
         p->pipe_latch[FE_LATCH][ii].tr_entry.src2_reg && \
         p->pipe_latch[MEM_LATCH][pw_i].op_id < p->pipe_latch[FE_LATCH][ii].op_id)
      {
        if(show_clash)
        {
          std::cout << "SRC2_CLASH_MEM_ID" << " ID4\n";
          std::cout << "Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "\
                    << p->pipe_latch[MEM_LATCH][pw_i].op_id << "\n";
        }
        // stall
        p->pipe_latch[FE_LATCH][ii].stall = true;

        if(ENABLE_MEM_FWD)
        {
          p->pipe_latch[FE_LATCH][ii].stall = false;
        }
        // break;
      }


      else if(p->pipe_latch[FE_LATCH][ii].valid && \
              p->pipe_latch[MEM_LATCH][pw_i].valid && \
              int(p->pipe_latch[MEM_LATCH][pw_i].tr_entry.cc_write) != 0 && \
              int(p->pipe_latch[FE_LATCH][ii].tr_entry.cc_read) != 0 && \
              p->pipe_latch[MEM_LATCH][pw_i].op_id < p->pipe_latch[FE_LATCH][ii].op_id)
      {
        if(show_clash)
        {
          std::cout << "CC_CLASH_MEM_ID" << " CC\n";
          std::cout << "Dep bw " << p->pipe_latch[FE_LATCH][ii].op_id << " "  \
                    << p->pipe_latch[MEM_LATCH][pw_i].op_id << "\n";
        }
        // stall
        p->pipe_latch[FE_LATCH][ii].stall = true;

        if(ENABLE_MEM_FWD)
        {
          p->pipe_latch[FE_LATCH][ii].stall = false;
        }

        // break;
      }
    } // pw_i for end



    if(p->pipe_latch[FE_LATCH][ii].stall == true && \
       (p->pipe_latch[FE_LATCH][ii].op_id < OLDEST_STALL_OP_ID) )
    {
      OLDEST_STALL_OP_ID = p->pipe_latch[FE_LATCH][ii].op_id;
    }



  } // ii for end



  for(int ii = 0; ii < PIPE_WIDTH; ii++)
  {
    if(p->pipe_latch[FE_LATCH][ii].op_id >= OLDEST_STALL_OP_ID)
    {
      p->pipe_latch[FE_LATCH][ii].stall = true;
      p->pipe_latch[FE_LATCH][ii].stall = true;
    }
    else
    {
      p->pipe_latch[FE_LATCH][ii].stall = false;
      p->pipe_latch[FE_LATCH][ii].stall = false;
    }
  }

  for(int ii=0; ii<PIPE_WIDTH; ii++)
  {
    if(p->pipe_latch[FE_LATCH][ii].valid == true && \
       p->pipe_latch[FE_LATCH][ii].stall == false)
    {

      p->pipe_latch[ID_LATCH][ii] = p->pipe_latch[FE_LATCH][ii];
      p->pipe_latch[ID_LATCH][ii].valid = true;
      p->pipe_latch[FE_LATCH][ii].valid = false;
    }
  }

  if(show_clash)
  {
    for(int ii = 0; ii < PIPE_WIDTH; ii++)
    {
      std::cout << p->pipe_latch[FE_LATCH][ii].stall << " ";
    }
    std::cout << " " << OLDEST_STALL_OP_ID << "\n";
  }

  // my code end

}

//--------------------------------------------------------------------//

void pipe_cycle_FE(Pipeline *p){
  int ii;
  Pipeline_Latch fetch_op;
  bool tr_read_success;

  for(ii=0; ii<PIPE_WIDTH; ii++)
  {
    // if(verbose)
    // {
    //   std::cout << p->pipe_latch[FE_LATCH][ii].stall << "\n";
    // }
    // if(!p->pipe_latch[FE_LATCH][ii].stall) std::cout << "HAHAHA FALSE" << "\n";

    // my code start
    // if FE_LATCH is invalid AND
    // if FE_LATCH is not stalled
    // then
    //  fetch instruction
    //  make FE_LATCH valid

    if(p->pipe_latch[FE_LATCH][ii].valid == false && \
       (p->pipe_latch[FE_LATCH][ii].stall == false))
    {

      pipe_get_fetch_op(p, &fetch_op);

      if(BPRED_POLICY){
        pipe_check_bpred(p, &fetch_op);
      }

      // copy the op in FE LATCH
      if(fetch_op.op_id < p->halt_op_id)
      {
        p->pipe_latch[FE_LATCH][ii]=fetch_op;
        p->pipe_latch[FE_LATCH][ii].valid = true;
      }
      // std::cout << p->pipe_latch[FE_LATCH][ii].op_id << "\n";
      // std::cout << "UNSTALLED FE\n";
      // unstalled++;
    }
    else
    {
      // std::cout << "STALLED FE\n";
      // stalled++;
    }
  }

}


//--------------------------------------------------------------------//

void pipe_check_bpred(Pipeline *p, Pipeline_Latch *fetch_op){
  // call branch predictor here, if mispred then mark in fetch_op
  // update the predictor instantly
  // stall fetch using the flag p->fetch_cbr_stall
}

//--------------------------------------------------------------------//
