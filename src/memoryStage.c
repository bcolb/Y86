#include <stdio.h>
#include "instructions.h"
#include "bool.h"
#include "stalling.h"
#include "status.h"
#include "forwarding.h"
#include "memoryStage.h"
#include "tools.h"
#include "memory.h"

/*
 * M register holds the input for the memory stage.
 */
static mregister M;

/*
 * Function: memoryStage
 * Description: Controls the flow for the memory stage.
 * Params: none
 * Returns: none
 * Modifies: m_valM
 */
void memoryStage(bool troubleshoot, forwardType *forward, statusType *status, stallType *stall)
{
  // get stignals for bubble and stall
  bool W_stall = getWStall(status);
  bool W_bubble = 0;
  bool mem_error = FALSE;
  unsigned int m_valM = memRead(&mem_error);
  memWrite(&mem_error);
  // update m_stat
  unsigned int m_stat = getMStat(mem_error);
  // update for stalling and bubbling
  stall->M_icode = M.icode;
  // update for status
  status->m_stat = m_stat;
  // Update for forwarding
  forward->M_dstM = M.dstM;
  forward->m_valM = m_valM;
  forward->M_dstE = M.dstE;
  forward->M_valE = M.valE;
  forward->M_Cnd = M.Cnd;
  forward->M_icode = M.icode;
  forward->M_valA = M.valA;
  // Print Statements for troubleshooting
  if(troubleshoot){
    printf("M  M.stat: %x M.icode: %x M.Cnd: %x M.valE: %x, M.valA: %x M.dstE: %x M.dstM: %x m_valM: %x \n", M.stat, M.icode, M.Cnd, M.valE, M.valA, M.dstE, M.dstM, m_valM);
  }
  // Update the W register only if it isn't stalled
  if(!W_stall){
    updateWregister(m_stat, M.icode, M.valE, m_valM, M.dstE, M.dstM);
  }
}

/* Function: getWStall
 * Description: Determines whether or not the W register needs to be stalled
 * Params: *status - a pointer to the status struct
 * Returns: bool W_stall
 * Modifies: none
 */
bool getWStall(statusType *status){
  if(status->W_stat == SADR || status->W_stat == SINS || status->W_stat == SHLT){
    return TRUE;
  } else {
    return FALSE;
  }
}

/* Function: getMStat
 * Description: gets the stat value for the M register
 * Params: mem_error
 * Returns: m_stat
 * Modifies: none  
 */
unsigned int getMStat(bool mem_error){
  if(mem_error){
    // printf("Setting MStat to SADR\n");
    return SADR;
  } else {
    // printf("Setting MStat to M.Stat\n");
    return M.stat;
  }
}
/* Function: memRead
 * Description: if required gets value located in memory
 * Params: *mem_error
 * Returns: m_valM
 * Modifiers: none
 */
unsigned int memRead(bool *mem_error)
{
  if(M.icode == MRMOVL){
    unsigned int theReturn =  getWord(M.valE, mem_error);
    return theReturn;
  } else if(M.icode == POPL || M.icode == RET){
    return getWord(M.valA, mem_error);
  } else {
    return 0x0;
  }
}

/* Function: memWrite
 * Description: Writes value to memory if necessary
 * Params: *mem_error
 * Returns: none
 * Modifiers: none
 */
unsigned int memWrite(bool *mem_error)
{
  if(M.icode == RMMOVL || M.icode == PUSHL){
    putWord(M.valE, M.valA, mem_error);
  } else if(M.icode == CALL){
    putWord(M.valE, M.valA, mem_error);
  }
}

/*
 * Function: updateMregister
 * Description: Updates the M register values from Execute Stage.
 * Params: stat, icode, Cnd, valE, valA, dstE, dstM
 * Returns: none
 * Modifiers: M
 */
void updateMregister(unsigned int stat, unsigned int icode, unsigned int Cnd, unsigned int valE,unsigned int valA, unsigned int dstE, unsigned int dstM)
{
    M.stat = stat;
    M.icode = icode;
    M.Cnd = Cnd;
    M.valE = valE;
    M.valA = valA;
    M.dstE = dstE;
    M.dstM = dstM;
}

/* Function: getMregister
 * Description: Returns a copy of the M register
 * Params: none
 * Returns: mregister
 * Modifies: none
 */
mregister getMregister()
{
    return M;
}

/* Function: clearMregister
 * Params: none
 * Returns: none
 * Modifies: M
 */ 
void clearMregister()
{
    clearBuffer((char *) &M, sizeof(M));
    M.icode = NOP;
    M.stat = SAOK;
}
