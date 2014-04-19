#include <stdio.h>
#include "bool.h"
#include "stalling.h"
#include "forwarding.h"
#include "status.h"
#include "decodeStage.h"
#include "tools.h"
#include "instructions.h"
#include "executeStage.h"
#include "registers.h"

/*
 * D register holds the input for the decode stage.
 */
static dregister D;

/* Function: decodeStage
 * Description: Controls the decode stage execution.
 * Params: none
 * Returns: none
 * Modifies: d_valA, d_valB, d_rvalA, d_valE, d_dstE, d_dstM, d_srcA, d_srcB
 */
void decodeStage(bool troubleshoot, forwardType forward, stallType *stall)
{
  // get and set the proper values for the decode stage
  unsigned int d_srcA = getSrcA();
  unsigned int d_srcB = getSrcB();
  unsigned int d_rvalA = getDrvalA(d_srcA);
  unsigned int d_rvalB = getDrvalB(d_srcB);
  unsigned int d_valA = selectFwdA(d_srcA, d_rvalA, forward);
  unsigned int d_valB = forwardB(d_srcB, d_rvalB, forward);
  unsigned int d_dstE = getDstE();
  unsigned int d_dstM = getDstM();
  // update for stalling and bubbling
  stall->d_srcA = d_srcA;
  stall->d_srcB = d_srcB;
  stall->D_icode = D.icode;
  bool E_stall = 0;
  bool E_bubble = getEBubble(stall);
  // print statements for troubleshooting
  if(troubleshoot){
    printf("D  D.stat: %x D.icode: %x D.ifun: %x D.rA: %x D.rB %x D.valC: %x D.valP: %x d_srcA: %x d_srcB: %x d_rvalA: %x d_valA: %x d_valB: %x d_dstE: %x d_dstM: %x\n", D.stat, D.icode, D.ifun, D.rA, D.rB, D.valC, D.valP, d_srcA, d_srcB, d_rvalA, d_valA, d_valB, d_dstE, d_dstM);
  }
  if(E_bubble){
    updateEregister(D.stat, NOP, FNONE, FNONE, FNONE, FNONE, RNONE, RNONE, RNONE, RNONE);
  } else {
    updateEregister(D.stat, D.icode, D.ifun, D.valC, d_valA, d_valB, d_dstE, d_dstM, d_srcA, d_srcB);
  }
}

/* Function: getBubble
 * Description: retrieves the bubble from stallType
 * Params: stallType 
 * Returns: boolean
 * Modifies: nothing
 */
bool getEBubble(stallType *stall){
  if(stall->E_icode == JMP && !stall->e_Cnd){
    // mispredicted branch
    return TRUE;
  } else if((stall->E_icode == MRMOVL || stall->E_icode == POPL) && (stall->E_dstM == stall->d_srcA || stall->E_dstM == stall->d_srcB)) {
    // conditions for a load/use hazard
    return TRUE;
  } else {
    return FALSE;
  }
}

/* Function: getDrvalA
 * Description: Gets the DrvalA from getRegister method.
 * Params: d_srcA
 * Returns: unsigned int
 * Modifies: nothing
 */
unsigned int getDrvalA(unsigned int d_srcA)
{
  if(D.icode == OPL || D.icode == RRMOVL || D.icode == MRMOVL || D.icode == RMMOVL || D.icode == PUSHL || D.icode == POPL){
    return getRegister(d_srcA);
  } else {
    return 0;
  }
}

/* Function: getDrvalB
 * Description: Gets the DrvalB from getRegister method.
 * Params: d_srcB
 * Returns: unsigned int
 * Modifies: nothing
 */
unsigned int getDrvalB(unsigned int d_srcB)
{
  if(D.icode == OPL || D.icode == IRMOVL || D.icode == MRMOVL || D.icode == RMMOVL || D.icode == PUSHL || D.icode == POPL){
    return getRegister(d_srcB);
  } else {
    return 0;
  }
}

/* Function: getSrcA
 * Description: Gets the appropriate value for srcA.
 * Params: none
 * Returns: unsigned int 
 * Modifies: nothing
 */
unsigned int getSrcA()
{
  if(D.icode == RRMOVL || D.icode == RMMOVL || D.icode == OPL || D.icode == PUSHL){
    return D.rA;
  } else if(D.icode == POPL || D.icode == RET){
    return RESP;
  } else {
    return RNONE;
  }
}

/* Function: getSrcB
 * Description: Gets the appropriate srcB value.
 * Params: none
 * Returns: unsigned int 
 * Modifies: nothing
 */
unsigned int getSrcB()
{
  if(D.icode == OPL || D.icode == RMMOVL || D.icode == MRMOVL){
    return D.rB;
  } else if(D.icode == PUSHL || D.icode == POPL || D.icode == CALL || D.icode == RET){
    return RESP;
  } else {
    return RNONE;
  }
}

/* Function: getDstE
 * Description: Gets the appropriate value for dstE.
 * Params: none
 * Returns: unsigned int 
 * Modifies: nothing
 */
unsigned int getDstE()
{
  if(D.icode == RRMOVL || D.icode == IRMOVL || D.icode == OPL){
    return D.rB;
  } else if(D.icode == PUSHL || D.icode == POPL || D.icode == CALL || D.icode == RET){
    return RESP;
  } else {
    return RNONE;
  }
}

/* Function: getDstM
 * Description: Gets the appropriate dstM
 * Params: forewardType foreward
 * Returns: unsigned int
 * Modifies: nothing
 */
unsigned int getDstM(forwardType forward)
{
  if(D.icode == MRMOVL || D.icode == POPL){
    return D.rA;
  } else {
    return RNONE;
  }
}

/* Function: selectFwdA
 * Description: Chooses the correct value for fwdA block
 * Params: unsigned int d_srcA, unsigned int d_rvalA, forwardType forward
 * Returns: unsigned int 
 * Modifies: nothing
 */
unsigned int selectFwdA(unsigned int d_srcA, unsigned int d_rvalA, forwardType forward)
{
  if(D.icode == CALL || D.icode == JMP){
    return D.valP;
  } else if(d_srcA == RNONE){
    return FNONE;
  } else if(d_srcA == forward.e_dstE){
    return forward.e_valE;
  } else if(d_srcA == forward.M_dstM){
    return forward.m_valM;
  } else if(d_srcA == forward.M_dstE){
    return forward.M_valE;
  } else if(d_srcA == forward.W_dstM){
    return d_rvalA;
  } else if(d_srcA == forward.W_dstE){
    return forward.W_valE;
  } else {
    return d_rvalA;
  }
}

/* Function: forwardB
 * Description: Chooses the correct value for fwdB block
 * Params: unsigned int d_srcB, unsigned int d_rvalB, forwardType forward
 * Returns: unsigned int 
 * Modifies: nothing
 */
unsigned int forwardB(unsigned int d_srcB, unsigned int d_rvalB, forwardType forward)
{
  if(d_srcB == RNONE){
    return 0;
  } 
  if(d_srcB == forward.e_dstE){
    return forward.e_valE;
  } else if(d_srcB == forward.M_dstM){
    return forward.m_valM;
  } else if(d_srcB == forward.M_dstE){
    return forward.M_valE;
  } else if(d_srcB == forward.W_dstM){
    return forward.W_valM;
  } else if(d_srcB == forward.W_dstE){
    return forward.W_valE;
  } else {
    return d_rvalB;
  }
}

/* Function: updateDregister
 * Description: Updates the D register.
 * Params: stat, icode, ifun, rA, rB, valC, valP
 * Returns: none
 * Modifies: D
 */
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP){
  D.stat = stat;
  D.icode = icode;
  D.ifun = ifun;
  D.rA = rA;
  D.rB = rB;
  D.valC = valC;
  D.valP = valP;
}

/* Function: getDregister
 * Description: Returns a copy of the D register.
 * Params: none
 * Returns: dregister
 * Modifies: none
 */
dregister getDregister()
{
    return D;
}

/* Function: clearDregister
 * Description: Clears the D register.
 * Params: none
 * Returns: none
 * Modifies: D 
 */
void clearDregister()
{
    clearBuffer((char *) &D, sizeof(D));
    D.icode = NOP;
    D.stat = SAOK;
}
