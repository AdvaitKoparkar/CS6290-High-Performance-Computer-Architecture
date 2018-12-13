#include <stdio.h>
#include <assert.h>

#include "rob.h"


extern int32_t NUM_ROB_ENTRIES;

/////////////////////////////////////////////////////////////
// Init function initializes the ROB
/////////////////////////////////////////////////////////////

ROB* ROB_init(void){
  int ii;
  ROB *t = (ROB *) calloc (1, sizeof (ROB));
  for(ii=0; ii<NUM_ROB_ENTRIES; ii++){
    t->ROB_Entries[ii].valid=false;
    t->ROB_Entries[ii].ready=false;
  }
  t->head_ptr=0;
  t->tail_ptr=0;
  return t;
}

/////////////////////////////////////////////////////////////
// Print State
/////////////////////////////////////////////////////////////
void ROB_print_state(ROB *t){
 int ii = 0;
  printf("Printing ROB \n");
  printf("Entry  Inst   Valid   ready\n");
  for(ii = 0; ii < NUM_ROB_ENTRIES; ii++) {
    printf("%5d ::  %d\t", ii, (int)t->ROB_Entries[ii].inst.inst_num);
    printf(" %5d\t", t->ROB_Entries[ii].valid);
    printf(" %5d\n", t->ROB_Entries[ii].ready);
  }
  printf("\n");
}

/////////////////////////////////////////////////////////////
//------- DO NOT CHANGE THE CODE ABOVE THIS LINE -----------
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// If there is space in ROB return true, else false
/////////////////////////////////////////////////////////////

bool ROB_check_space(ROB *t)
{
  for(int ii=0; ii < NUM_ROB_ENTRIES; ii++)
  {
	   if(t->ROB_Entries[ii].valid == false)
     {
		   return true;
     }
  }
  return false;
}

/////////////////////////////////////////////////////////////
// insert entry at tail, increment tail (do check_space first)
/////////////////////////////////////////////////////////////

int ROB_insert(ROB *t, Inst_Info inst){
  // my code start
  // if empty
  // return -1
  // else
  // insert at tail
  // make vaid
  // make ready false
  // increment tail
  // return prf_id
  int prf_id = -1;

  if(!ROB_check_space(t))
  {
    return -1;
  }

  prf_id = t->tail_ptr;
  t->ROB_Entries[t->tail_ptr].valid = true;
  t->ROB_Entries[t->tail_ptr].ready = false;
  t->ROB_Entries[t->tail_ptr].inst = inst;
  t->ROB_Entries[t->tail_ptr].inst.dr_tag = prf_id;

  if(t->tail_ptr < NUM_ROB_ENTRIES - 1)
  {
    t->tail_ptr++;
  }
  else if(t->tail_ptr == NUM_ROB_ENTRIES - 1)
  {
    t->tail_ptr = 0;
  }
  return prf_id;
  // my code end
}


/////////////////////////////////////////////////////////////
// Once an instruction finishes execution, mark rob entry as done
/////////////////////////////////////////////////////////////

void ROB_mark_ready(ROB *t, Inst_Info inst){
  // my code start
  // find inst
  // mark ready
  for(int rob_ind = 0; rob_ind < NUM_ROB_ENTRIES; rob_ind++)
  {
    if(t->ROB_Entries[rob_ind].inst.inst_num == inst.inst_num && \
       t->ROB_Entries[rob_ind].valid == true)
    {
      t->ROB_Entries[rob_ind].ready = true;
      break;
    }
  }
  // my code end
}

/////////////////////////////////////////////////////////////
// Find whether the prf-rob entry is ready
/////////////////////////////////////////////////////////////

bool ROB_check_ready(ROB *t, int tag){
  // my code start
  // if tag is ready
  // return true
  // else
  // return false
  if(t->ROB_Entries[tag].ready) // valid??
  {
    return true;
  }
  else
  {
    return false;
  }
  // my code end
}


/////////////////////////////////////////////////////////////
// Check if the oldest ROB entry is ready for commit
/////////////////////////////////////////////////////////////

bool ROB_check_head(ROB *t){
  // my code start
  // if head is ready and valid
  // return true
  // else
  // return false
  if(t->ROB_Entries[t->head_ptr].valid == true && \
     t->ROB_Entries[t->head_ptr].ready == true)
  {
    return true;
  }
  else
  {
    return false;
  }
  // my code end
}

/////////////////////////////////////////////////////////////
// Remove oldest entry from ROB (after ROB_check_head)
/////////////////////////////////////////////////////////////

Inst_Info ROB_remove_head(ROB *t){
  // my code start
  // check if head is ready
  // if not ready
  // return
  // else
  // make head invalid
  // temp_inst = head->inst
  // if head_ptr == end of ROB REMOVED && tail != 0
  // head = beginning of ROB
  //   if tailP-tr is also zero
  //     increment tail;
  // else if head_ptr < end of rob && tail != head + 1
  // head_ptr++
  if(!ROB_check_head(t))
  {
    // what to return?
    Inst_Info void_inst;
    return void_inst;
  }
  else
  {
    t->ROB_Entries[t->head_ptr].valid = false;
    Inst_Info temp = t->ROB_Entries[t->head_ptr].inst;
    if(t->head_ptr == NUM_ROB_ENTRIES - 1)
    {
      t->head_ptr = 0;
      if(t->tail_ptr == 0)
      {
        t->tail_ptr++;
      }
    }
    else if(t->head_ptr < NUM_ROB_ENTRIES - 1)
    {
      t->head_ptr++;
      if(t->head_ptr == t->tail_ptr)
      {
        t->tail_ptr++;
      }
    }
    return temp;
  }
  // my code end
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
