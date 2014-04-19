#include <stdio.h>
#include "registers.h"
#include "bool.h"
#include "stalling.h"
#include "status.h"
#include "forwarding.h"
#include "executeStage.h"
#include "tools.h"
#include "instructions.h"
/* 
 * E register holds the input for the execute stage.
 */
static eregister E;
static functionptr fp[16];

/* Function: executeStage
 * Description: Controls the flow of the execute stage.
 * Params: none
 * Returns: none
 * Modifies: e_Cnd, e_valE
 */
void executeStage(bool troubleshoot, forwardType *forward, statusType status, stallType *stall)
{
  bool M_stall = 0;
  bool M_bubble = getMBubble(status);
  bool e_Cnd = getCnd();
  unsigned int e_valE = E.valC;
  unsigned int e_dstE = getEdstE(e_Cnd);
  // check for error and opcode
  if(E.icode == OPL){
    e_valE = performOpl(status);
  } else {
    e_valE = fp[E.icode]();
  }
  // Update for forwarding
  forward->e_dstE = e_dstE;
  forward->e_valE = e_valE;
  // Update for stalling and bubbling
  stall->E_dstM = E.dstM;
  stall->e_Cnd = e_Cnd;
  stall->E_icode = E.icode;
  // print statements for troubleshooting
  if(troubleshoot){
    printf("E  E.stat: %x E.icode: %x E.ifun: %x E.valC: %x E.valA: %x E.valB: %x E.dstE: %x E.dstM: %x E.srcA: %x E.srcB %x e_Cnd: %x e_valE: %x \n", E.stat, E.icode, E.ifun, E.valC, E.valA, E.valB, E.dstE, E.dstM, E.srcA, E.srcB, e_Cnd, e_valE);
  }
  // Update the Mregister
  if(M_bubble){
    updateMregister(E.stat, NOP, FNONE, FNONE, FNONE, RNONE, RNONE);
  } else {
    updateMregister(E.stat, E.icode, e_Cnd, e_valE, E.valA, e_dstE, E.dstM);
  }
}

/* Function: getMBubble
 * Description: returns true if the m register needs to be bubbled
 * Params: status - stat from other registers  
 * Returns: true if bubble of m register is neccessary
 * Modifies: none
 */
bool getMBubble(statusType status){
  if(status.m_stat == SADR || status.m_stat == SINS || status.m_stat == SHLT || status.W_stat == SADR || status.W_stat == SINS || status.W_stat == SHLT){
    return TRUE;
  } else {
    return FALSE;
  }
}


/* Function: initializeFuncPtrArray
 * Description: Initializes the function pointer array to be used 
                in the Execute Stage.
 * Params: none
 * Returns: none
 * Modifies: p
 */
void initializeFuncPtrArray()
{
  // Will initialize the array of function pointers
  int i;
  for(i = 0; i < 16; i++)
    {
      fp[i] = zeroFunc;
    }
  fp[DUMP] = dumpFunc;
  fp[IRMOVL] = performIrmovl;
  fp[RRMOVL] = performCmov; // RRMOVL same as CMOVXX
  fp[RMMOVL] = performRmmovl;
  fp[MRMOVL] = performMrmovl;
  fp[PUSHL] = performPushl;
  fp[POPL] = performPopl;
  fp[CALL] = performCall;
  fp[RET] = performRet;
}

/* Function: performCall                            
 * Description: calculates valE for a call instruction             
 * Params: none
 * Returns: E_valB - 4                        
 * Modifies: none                                  
 */
unsigned int performCall()
{
  return E.valB + (-4);
}

/* Function: performRet
 * Description: calculates e_valE for a return instruction              
 * Params: none
 * Returns: E_valB + 4         
 * Modifies: none                         
 */
unsigned int performRet()
{
  return E.valB + 4;
}

/* Function: performPushl
 * Description: calculates the e_valE for a PUSH instruction
 * Params: none
 * Returns: E_valB - 4
 * Modifies: none
 */
unsigned int performPushl()
{
  return E.valB + (-4);
}

/* Function: performPopl
 * Description: Calculates the e_valE for a POP instruction
 * Params: none
 * Returns: E_valB + 4     
 * Modifies: none
 */
unsigned int performPopl()
{
  return E.valB + 4;
}

/* Function: performCmov
 * Description: Calculates e_valE for RRMOVL and CMOVXX
 * Params: none
 * Returns: E_valA
 * Modifies: none
 */
unsigned int performCmov()
{
  return 0 + E.valA;
}

/* Function: performRmmovl        
 * Description: calculates the value for e_valE for a RMMOVL instruction
 * Params: none
 * Returns: E_valB + E_valC     
 * Modifies: none
 */
unsigned int performRmmovl()
{
  return E.valB + E.valC;
}

/* Function: performMrmovl
 * Description: calcualtes the value for e_valE for a MRMOVL instruction
 * Params: none
 * Returns: E_valB + E_valC     
 * Modifies: none
 */
unsigned int performMrmovl()
{
  return E.valB + E.valC;
}

/* Function: performOpl
 * Description: performs add, sub, etc.
 * Params: status - stat codes from other registers
 * Returns: e_valE
 * Modifies: Condition Codes
 */
unsigned int performOpl(statusType status)
{
  bool set_cc = TRUE;
  if(status.m_stat == SADR || status.m_stat == SINS || status.m_stat == SHLT || status.W_stat == SADR || status.W_stat == SINS || status.W_stat == SHLT){
    set_cc = FALSE;
  }
  // save condition codes                              
  unsigned int zero, one, two;                                                
  zero = getCC(0);                                   
  one = getCC(0);                                 
  two = getCC(0);
  int x = E.valB;
  int y = E.valA;
  unsigned int result;
  if(E.ifun == 0){
    result = x + y;
    signed int sResult = (signed int) result;
    if((x > 0 && y > 0 && sResult <= 0) || (x < 0 && y < 0 && sResult >= 0)){
      setCC(0, 1);
    } else {
      setCC(0, 0);
    }
  } else if(E.ifun == 1){
    result = x - y;
    signed int sResult = (signed int) result;
    if((x > 0 && y < 0 && sResult <= 0) ||(x < 0 && y > 0 && sResult >= 0)){
      setCC(0, 1);
    } else {
      setCC(0, 0);
    }
  } else if(E.ifun == 2){
    result = x & y;
    // Set OF
    setCC(0, 0);
  } else {
    result = x ^ y;
    // Set OF
    setCC(0, 0);
  }
  // Set ZF
  setCC(2, (result == 0));
  // Set SF
  signed int signedResult = (signed int) result;
  setCC(1, (signedResult < 0));
  // restore condition codes if set_cc is false
  if(!set_cc){
    setCC(0, zero);                                                               
    setCC(1, one);                                                                    
    setCC(2, two);
  }  
  return result;
}

/* Function: performIrmovl
 * Description: returns the value for e_valE
 * Params: none
 * Returns: e_valE
 * Modifies: none
 */
unsigned int performIrmovl()
{
  return E.valC;
}

/*
 * Function: zeroFunc
 * Description: Returns zero, is the default function for the pointer array functions.
 * Params: none
 * Returns: FNONE
 * Modifies: none
 */ 
unsigned int zeroFunc()
{
  return FNONE;
}

/*
 * Function: dumpFunc
 * Description: Returns the valC.
 * Params: none
 * Returns: E.valC
 * Modifies: none
 */
unsigned int dumpFunc()
{
  return E.valC;
}

/* Function: getEdstE
 * Description: gets the value for e_dstE
 * Params: e_Cnd
 * Returns: unsigned int d_dstE
 * Modifies: none
 */
unsigned int getEdstE(unsigned int e_Cnd)
{
  if(E.icode == RRMOVL && !e_Cnd){
    return RNONE;
  } else {
    return E.dstE;
  }
}

/* Function: getCnd
 * Description: Determines the condition code
 * Params: none
 * Returns: the condition code
 * Modifies: none
 */
bool getCnd()
{
  if(E.icode != RRMOVL && E.icode != JMP){
    return FALSE;
  }
  bool cnd;
  unsigned int over = getCC(0);
  unsigned int sign = getCC(1);
  unsigned int zero = getCC(2);
  if(E.ifun == 0){          // RRMOVL
    cnd = TRUE;
  } else if(E.ifun == 1){   // CMOVLE
    cnd = (sign ^ over) | zero;
  } else if(E.ifun == 2){   // CMOVL 
    cnd = (sign ^ over);
  } else if(E.ifun == 3){   // CMOVE/JE
    cnd = zero;
  } else if(E.ifun == 4){   // CMOVNE
    cnd = !zero;
  } else if(E.ifun == 5){   // CMOVGE
    cnd = !(sign ^ over);
  } else if(E.ifun == 6){   // CMOVG
    cnd = !(sign ^ over) &&  !zero;
  }
  return cnd;
}

/*
 * Function: updateEregister
 * Description: Updates the values of the E register.
 * Params: stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB
 * Returns: none
 * Modifies: E
 */
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int valC, unsigned int valA, unsigned int valB, unsigned int dstE, unsigned int dstM, unsigned int srcA, unsigned int srcB)
{
  E.stat = stat;
  E.icode = icode;
  E.ifun = ifun;
  E.valC = valC;
  E.valA = valA;
  E.valB = valB;
  E.dstE = dstE;
  E.dstM = dstM;
  E.srcA = srcA;
  E.srcB = srcB;
}

/* Function: getEregister
 * Description: Returns a copy of the E register
 * Params: none
 * Returns: eregister
 * Modifies: none
 */
eregister getEregister()
{
    return E;
}

/* Function: clearEregister
 * Params: none
 * Returns: none
 * Modifies: E
 */ 
void clearEregister()
{
    clearBuffer((char *) &E, sizeof(E));
    E.icode = NOP;
    E.stat = SAOK;
}
