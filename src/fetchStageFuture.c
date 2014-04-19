#include "bool.h"
#include "fetchStage.h"
#include "tools.h"
#include "instructions.h"
#include <stdio.h>
/* F register holds the input for the fetch stage. 
 * It is only accessible from this file. (static)
 */
static fregister F;
void fetchStage()
{
    // Get the current PC
    unsigned int f_pc = selectPC();
    // Get and split the icode and ifun
    bool error;
    char ibyte = getByte(f_pc, &error);
    unsigned int f_icode = (unsigned int) ((ibyte>>4) & 0x0f);
    unsigned int f_ifun = (unsigned int) (ibyte & 0x0f);
    
    unsigned int f_stat;
    unsigned int f_rA;
    unsigned int f_rB;
    unsigned int f_valC;
    unsigned int f_valP;
    if(error){
      f_icode = NOP;
      f_stat = SINS;
      f_rA = FNONE;
      f_rB = FNONE;
      f_valC = FNONE;      f_valP = FNONE;
      updateDregister(f_stat, f_icode, f_ifun, f_rA, f_rB, f_valC, f_valP);
    } else {
      f_stat = getStat(f_icode);
    }
    
    if (needsRegs){
        f_rA = getRA(f_pc);
        f_rB = getRB(f_pc);
    }
    else{ //if (!needsRegs)
        f_rA = RNONE;//maybe needs to be RNONE (0xf)
        f_rB = RNONE;//does it matter? no other stage will use this...
    }
    
    if (needsValC){
        f_valC = getValC(f_pc);
    }
    
    // Increment PC
    
    if(f_icode = JMP || f_icode== CALL || f_icode == JL || f_icode == JE || f_icode == JNE || f_icode == JGE || f_icode == JG){
        f_valP = pcInc(f_icode, f_pc);
    }
    else{
        f_valP = predictPC(f_icode, f_valC);//send offset and code?...
    }
    
    f_valP = pcInc(f_icode, f_pc);
    F.predPC = f_valP;
    printf("fetchStage f_icode: %x f_ifun: %x f_stat: %x\n", f_icode, f_ifun, f_stat);
    updateDregister(f_stat, f_icode, f_ifun, f_rA, f_rB, f_valC, f_valP);
}
    /*
     * Description: begins align with helper function in
     * Param:f_icode - the instruction to evaluate
     * Return: boolean T/F indicating yes or no
     * Modifies nothing
     */
bool needsRegs(unsigned int f_icode)
{
    if (f_icode == RRMOVL || f_icode == IRMOVL || f_icode == MRMOVL || f_icode == OPL)
        return TRUE;
        else
            return FALSE;
}

    /* Description: to see if you need an offset
     * Param: f_icode
     * Return: boolean T/F if it does (or not) need valC
     */ 
bool needsValC(unsigned int f_icode)
{
    if (f_icode == JMP || f_icode == IRMOVL || f_icode == MRMOVL || f_icode == RMMOVL || f_icode == CALL)
        return TRUE;
        else
            return FALSE; 
}
 

unsigned int predictPC(unsigned int f_icode, unsigned int f_valC)
{
    if(f_icode == JMP || f_icode== CALL || f_icode == JL || f_icode == JE || f_icode == JNE || f_icode == JGE || f_icode == JG){
       return f_valC;
    }else{
        return f_valP;
    }
    
}
int selectPC(unsigned int f_icode)
{
    /*if(M.icode == JMP) && !M.Cnd)
        return M.valA;//lab7
    if (W_icode == RET)
        return W.valM;//lab8ish*/

    return F.predPC;
}

/**this method will be used when no branch instruction
 * has been read to set f_valP for propogation thru
 * the pipeline. If there is an always taken scenario
 * f_valP should be set by the predictPC method.
 */
unsigned int pcInc(unsigned int f_icode, unsigned int f_pc)
{
  unsigned int f_valP;
  if(f_icode == NOP){
    f_valP = f_pc + 1;
  } else if (f_icode == HALT) {
    f_valP = f_pc + 1;
  } else if (f_icode == DUMP) {
    f_valP = f_pc + 5;
  } else if (f_icode == IRMOVL) {
      //these type instructions should have
      //both needRegs and needValC set to true
  } else if (f_icode == RMMOVL) {
  } else if (f_icode == MRMOVL) {
  } else if (f_icode == OPL) {
  } else if (f_icode == JMP) {
      //use predictPC to update F.PC
  } else if (f_icode == CALL) {
  } else if (f_icode == RET) {
  } else if (f_icode == PUSHL) {
  } else if (f_icode == POPL) {
  } else {
    f_valP = f_pc + 2;
  }
  return f_valP;
}

void setPc(unsigned int f_newPC)
{
  F.predPC = f_newPC;
}

unsigned int getStat(unsigned int f_icode)
{
  if(f_icode == HALT){
    return SADR;// no hardcode 2 SADR 
  } else if(f_icode > 0xC) {
    return SHLT;//SHLT=4
  } else {
    return SAOK;//SOAK=1
  }
}

unsigned int getRA(unsigned int f_pc)
{
    //eventually this will bit extract f_pc
  return RNONE;
}

unsigned int getRB(unsigned int f_pc)
{:
  return RNONE;
}

unsigned int getValC(unsigned int f_pc)
{
  return 0x0;
}

/* Function: getFregister
 * Description: Returns a copy of the F register
 * Params: none
 * Returns: fregister
 * Modifies: none
 */
fregister getFregister()
{
    return F;
}

/* Function: clearFregister
 * Params: none
 * Returns: none
 * Modifies: F 
 */
void clearFregister()
{
    clearBuffer((char *) &F, sizeof(F));
}
