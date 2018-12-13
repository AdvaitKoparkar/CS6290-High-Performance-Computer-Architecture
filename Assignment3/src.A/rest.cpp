#include <stdio.h>
#include <assert.h>

#include "rest.h"

extern int32_t NUM_REST_ENTRIES;

/////////////////////////////////////////////////////////////
// Init function initializes the Reservation Station
/////////////////////////////////////////////////////////////

REST* REST_init(void){
  int ii;
  REST *t = (REST *) calloc (1, sizeof (REST));
  for(ii=0; ii<MAX_REST_ENTRIES; ii++){
    t->REST_Entries[ii].valid=false;
  }
  assert(NUM_REST_ENTRIES<=MAX_REST_ENTRIES);
  return t;
}

////////////////////////////////////////////////////////////
// Print State
/////////////////////////////////////////////////////////////
void REST_print_state(REST *t){
 int ii = 0;
  printf("Printing REST \n");
  printf("Entry  Inst Num  S1_tag S1_ready S2_tag S2_ready  Vld Scheduled\n");
  for(ii = 0; ii < NUM_REST_ENTRIES; ii++) {
    printf("%5d ::  \t\t%d\t", ii, (int)t->REST_Entries[ii].inst.inst_num);
    printf("%5d\t\t", t->REST_Entries[ii].inst.src1_tag);
    printf("%5d\t\t", t->REST_Entries[ii].inst.src1_ready);
    printf("%5d\t\t", t->REST_Entries[ii].inst.src2_tag);
    printf("%5d\t\t", t->REST_Entries[ii].inst.src2_ready);
    printf("%5d\t\t", t->REST_Entries[ii].valid);
    printf("%5d\n", t->REST_Entries[ii].scheduled);
    }
  printf("\n");
}

/////////////////////////////////////////////////////////////
//------- DO NOT CHANGE THE CODE ABOVE THIS LINE -----------
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// If space return true else return false
/////////////////////////////////////////////////////////////

bool  REST_check_space(REST *t){
  // my code start
  // for i in range(NUM_REST_ENTRIES)
  //  if invalid
  //   return true
  // end for
  // return false
  for(int rest_ind = 0; rest_ind < NUM_REST_ENTRIES; rest_ind++)
  {
    if(t->REST_Entries[rest_ind].valid == false)
    {
      return true;
    }
  }
  return false;
  // my code end
}

/////////////////////////////////////////////////////////////
// Insert an inst in REST, must do check_space first
/////////////////////////////////////////////////////////////

void  REST_insert(REST *t, Inst_Info inst){
  // my code start
  // if space not av
  //  return
  // else
  //  find invalid entry
  //  insert inst data
  //  return
  if(!REST_check_space(t))
  {
    return;
  }
  for(int rest_ind = 0; rest_ind < NUM_REST_ENTRIES; rest_ind++)
  {
    if(t->REST_Entries[rest_ind].valid == false)
    {
      t->REST_Entries[rest_ind].valid = true;
      t->REST_Entries[rest_ind].scheduled = false;
      t->REST_Entries[rest_ind].inst = inst;
      return;
    }
  }
  // my code end
}

/////////////////////////////////////////////////////////////
// When instruction finishes execution, remove from REST
/////////////////////////////////////////////////////////////

void  REST_remove(REST *t, Inst_Info inst){
  // my code starts
  // find corresponding inst
  // make rob invalid
  // return
  for(int rest_ind = 0; rest_ind < NUM_REST_ENTRIES; rest_ind++)
  {
    if(t->REST_Entries[rest_ind].inst.inst_num == inst.inst_num)
    {
      t->REST_Entries[rest_ind].valid = false;
      return;
    }
  }
  // my code ends
}

/////////////////////////////////////////////////////////////
// For broadcast of freshly ready tags, wakeup waiting inst
/////////////////////////////////////////////////////////////

void  REST_wakeup(REST *t, int tag){
  // my code start
  // for i in range NUM_REST_ENTRIES
  //  if REST_entry is valid AND
  //     REST_entry src1 or src2 == tag
  //    make that tag ready
  for(int rest_ind = 0; rest_ind < NUM_REST_ENTRIES; rest_ind++)
  {
    if(t->REST_Entries[rest_ind].valid == true && \
       t->REST_Entries[rest_ind].inst.src1_reg != -1 && \
       t->REST_Entries[rest_ind].inst.src1_tag == tag)
    {
      t->REST_Entries[rest_ind].inst.src1_ready = true;
    }

    if(t->REST_Entries[rest_ind].valid == true && \
       t->REST_Entries[rest_ind].inst.src2_reg != -1 && \
       t->REST_Entries[rest_ind].inst.src2_tag == tag)
    {
      t->REST_Entries[rest_ind].inst.src2_ready = true;
    }

  } // end for
  // my code end
}

/////////////////////////////////////////////////////////////
// When an instruction gets scheduled, mark REST entry as such
/////////////////////////////////////////////////////////////

void  REST_schedule(REST *t, Inst_Info inst){
  // my code start
  // find inst in rest
  // schedule
  for(int rest_ind = 0; rest_ind < NUM_REST_ENTRIES; rest_ind++)
  {
    if(inst.inst_num == t->REST_Entries[rest_ind].inst.inst_num)
    {
      t->REST_Entries[rest_ind].scheduled = true;
      break;
    }
  }
  // my code end
}
