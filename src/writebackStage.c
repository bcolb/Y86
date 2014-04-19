#include <stdio.h>
#include "bool.h"
#include "stalling.h"
#include "status.h"
#include "forwarding.h"
#include "writebackStage.h"
#include "tools.h"
#include "instructions.h"
#include "executeStage.h"

/* 
 * W register holds the input for the writeback stage. 
 */
static wregister W;

/* 
 * Function: writebackStage
 * Description: Controls the flow of writeback stage.
 * Params: none
 * Returns: boolean value
 * Modifies: W
 */
bool writebackStage(bool troubleshoot, forwardType *forward, statusType *status)
{
  // Check for and handle exceptions
  // Returning TRUE halts program execution
  if(W.icode == HALT){
    return TRUE;
  } else if(W.stat == SINS){
    printf("Invalid instruction\n");
    dumpProgramRegisters();
    dumpProcessorRegisters();
    dumpMemory();
    return TRUE;
  } else if(W.stat == SADR){
    printf("Invalid memory address\n");
    dumpProgramRegisters();
    dumpProcessorRegisters();
    dumpMemory();
    return TRUE;
  }

  // Update Forwarding
  forward->W_valE = W.valE;
  forward->W_dstE = W.dstE;
  forward->W_dstM = W.dstM;
  forward->W_valE = W.valE;
  forward->W_valM = W.valM;
  forward->W_icode = W.icode;

  // Set registers if required
  if(W.icode == OPL || W.icode == IRMOVL || W.icode == RRMOVL){
    setRegister(W.dstE, W.valE);
  } else if(W.icode == MRMOVL) {
    // Put valM into Register
    setRegister(W.dstM, W.valM);
  }

  // Just for POPL or PUSHL
  if(W.icode == POPL){
    setRegister(RESP, W.valE);
    setRegister(W.dstM, W.valM);
  }
  if(W.icode == PUSHL || W.icode == CALL || W.icode == RET){
    setRegister(RESP, W.valE);
  }

  // Update status
  status->W_stat = W.stat;

  // Print statements for troubleshooting
  if(troubleshoot){
    printf("W  W.stat: %x W.icode: %x W.valE: %x W.valM: %x W.dstE: %x W.dstM: %x \n", W.stat, W.icode, W.valE, W.valM, W.dstE, W.dstM);   
  }

  // Handle DUMP instruction
  if(W.icode == DUMP){
    if(!troubleshoot){
      performDump();
    }
  }

  // Return false, program continues execution
  return FALSE;
}

/* Function: updateWregister
 * Description: Updates the values for the W register.
 * Params: stat, icode, valE, valM, dstE, dstM
 * Returns: none
 * Modifies: W
 */
void updateWregister(unsigned int stat, unsigned int icode, unsigned int valE, unsigned int valM, unsigned int dstE, unsigned int dstM)
{
  W.stat = stat;
  W.icode = icode;
  W.valE = valE;
  W.valM = valM;
  W.dstE = dstE;
  W.dstM = dstM;
}

/* Function: performDump
 * Descriptoin: dumps the contents of Program Registers, and/or  Processor Registers, and/or Memory.
 * Params: none
 * Returns: none
 * Modifies: none
 */
void performDump()
{
  // printf("*****About To Perform Dump*****\n");
  // printf("stat: %x icode: %x valE: %x valM: %x dstE %x dstM %x\n", W.stat, W.icode, W.valE, W.valM, W.dstE, W.dstM);
  unsigned int w_valC = dumpFunc();
  // printf("valC: %x\n", w_valC);
  if((W.valE & 0x01) == 1)
    {
      dumpProgramRegisters();
    }
  if((W.valE & 0x02) == 2)
    {
      dumpProcessorRegisters();
    }
  if((W.valE & 0x04) == 4)
    {
      dumpMemory();
    }
}

/* Function: getWregister
 * Description: Returns a copy of the W register
 * Params: none
 * Returns: wregister
 * Modifies: none
 */
wregister getWregister()
{
    return W;
}

/* Function: clearWregister
 * Params: none
 * Returns: none
 * Modifies: W
 */ 
void clearWregister()
{
    clearBuffer((char *) &W, sizeof(W));
    W.icode = NOP;
    W.stat = SAOK;
}
