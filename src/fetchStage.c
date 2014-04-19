#include "bool.h"
#include "forwarding.h"
// #include "bubbling.h"
#include "stalling.h"
#include "fetchStage.h"
#include "tools.h"
#include "instructions.h"
#include <stdio.h>

/*
 * F register holds the input for the fetch stage.
 */
static fregister F;

/* Function: fetchStage 
 * Description: Controls the flow for the fetchStage of the YESS simulator.
 * Params: none          
 * Returns: none        
 * Modifies: F, f_pc, f_icode, f_ifun, f_stat, f_rA, f_rB, f_valC, f_valP, 
             f_stat   
 */
void fetchStage(bool troubleshoot, forwardType forward, stallType stall)
{
    // Get bool values for bubbling and stalling
    bool F_bubble = 0;
    bool F_stall = getFStall(stall);
    bool D_stall = getDStall(stall);
    bool D_bubble = getDBubble(stall);
    // Get the current PC
    unsigned int f_pc = selectPC(forward);
    // Get and split the icode and ifun
    bool error;
    char ibyte = getByte(f_pc, &error);
    unsigned int f_icode = (unsigned int) ((ibyte>>4) & 0x0f);
    bool valid_opcode = validInstruction(f_icode);
    // Checks for valid opcode
    unsigned int f_ifun = (unsigned int) (ibyte & 0x0f);
    unsigned int f_stat;
    unsigned int f_rA;
    unsigned int f_rB;
    unsigned int f_valC;
    unsigned int f_valP;
    // align
    if(needsRegIDs(f_icode)){
      char regByte = getByte((f_pc + 1), &error);
      f_rA = getRA(f_pc, f_icode, regByte);
      f_rB = getRB(f_pc, f_icode, regByte);
      if(f_icode == PUSHL || f_icode == POPL){
	f_rB = RNONE;
      }
    } else {
      f_rA = RNONE;
      f_rB = RNONE;
    }
    if(error){
      f_ifun = FNONE;
    }
    f_valC = getValC(f_pc, f_icode, f_ifun, error);
    // Increment PC - Check for stalling and if so keep valP at F_predPC
    if(!F_stall){
      f_valP = pcIncrement(f_icode, f_pc, valid_opcode);
      predictPC(f_valP, f_valC, f_icode);
    } else {
      f_valP = F.predPC;
    }
    // fstat
    f_stat = getStat(f_icode, error, valid_opcode);
    // Print statements for trouleshooting
    if(troubleshoot){
      printf("F  f_stat: %x f_icode: %x f_ifun: %x f_rA: %x f_rB %x f_valC %x f_valP %x\n", f_stat, f_icode, f_ifun, f_rA, f_rB, f_valC, f_valP);
    }
    // Check for bubble
    if(D_bubble){
      // insert NOP
      updateDregister(SAOK, NOP, FNONE, RNONE, RNONE, FNONE, FNONE);
    } else if (!D_stall){
      updateDregister(f_stat, f_icode, f_ifun, f_rA, f_rB, f_valC, f_valP);
    }
}  

/* Function: getFStall
 * Description: Determines whether or not the f stage needs to be stalled
 * Params: stat values from other registers
 * Returns: true if f stage needs to be stalled
 * Modifies: none
 */
bool getFStall(stallType stall){
  if((stall.E_icode == MRMOVL || stall.E_icode == POPL) && (stall.E_dstM == stall.d_srcA || stall.E_dstM == stall.d_srcB)){
    return TRUE;
  } else if(stall.D_icode == RET || stall.E_icode == RET || stall.M_icode == RET) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* Function: getDStall
 * Description: Determines whether or not the d register needs to be stalled
 * Params: stall - values from other registers
 * Returns: bool value
 * Modifies: none
 */
bool getDStall(stallType stall){
  if((stall.E_icode == MRMOVL || stall.E_icode == POPL) && (stall.E_dstM == stall.d_srcA || stall.E_dstM == stall.d_srcB)){
    return TRUE;
  } else {
    return FALSE;
  }
}

/* Function: getDBubble
 * Description: determines whether or not the the D register needs to be bubbled
 * Params: stall - values from other registers 
 * Returns: bool value
 * Modifies: none
 */
bool getDBubble(stallType stall){
  if(stall.E_icode == JMP && !stall.e_Cnd){
    return TRUE;
  } else if(!((stall.E_icode == MRMOVL || stall.E_icode == POPL) && (stall.E_dstM == stall.d_srcA || stall.E_dstM == stall.d_srcB)) && (stall.D_icode == RET || stall.E_icode == RET || stall.M_icode == RET)){
    return TRUE;
  } else {
    return FALSE;
  }
}

/* Function: validInstruction
 * Description: checks to see if the opcode is valid
 * Params: f_icode
 * Returns: true if valid, false otherwise
 * Modifies: none
 */
bool validInstruction(unsigned int f_icode){
  if(f_icode < 0 || f_icode > 12){
    return FALSE;
  } else {
    return TRUE;
  }
}

/* Function: needsRegIDs
 * Description: Returns true if register ids are needed by the instruction
 * Params: f_icode
 * Returns: true if needs register ids, false otherwise
 * Modifies: none
 */
bool needsRegIDs(unsigned int f_icode)
{
  if(f_icode == RRMOVL || f_icode == OPL || f_icode == PUSHL || f_icode == POPL || f_icode == IRMOVL || f_icode == RMMOVL || f_icode == MRMOVL){
    return TRUE;
  } else {
    return FALSE;
  }
}

/* Function: selectPC
 * Description: Selects the correct Program Counter
 * Params: none
 * Returns: F.predPC
 * Modifies: none
 */
unsigned int selectPC(forwardType forward)
{
  if(forward.M_icode == JMP && !forward.M_Cnd){
    return forward.M_valA;
  } else if (forward.W_icode == RET){
    return forward.W_valM;
  } else {
    return F.predPC;
  }
}

/* Function: pcIncrement
 * Description: Calculates and returns the program counter based on the icode.
 * Params: f_icode, f_pc
 * Returns: f_pc
 * Modifies: F.predPC 
 */
unsigned int pcIncrement(unsigned int f_icode, unsigned int f_pc, bool valid_opcode)
{
  unsigned int f_valP;
  if(!valid_opcode){
    F.predPC = F.predPC + 1;
    return F.predPC;
  }
  if(f_icode == NOP){
    f_valP = f_pc + 1;
  } else if (f_icode == HALT || f_icode == RET) {
    f_valP = f_pc + 1;
  } else if (f_icode == DUMP || f_icode == JMP || f_icode == CALL) {
    f_valP = f_pc + 5;
  } else if (f_icode == IRMOVL || f_icode == RMMOVL || f_icode == MRMOVL){
    f_valP = f_pc + 6;
  } else if (f_icode == OPL || f_icode == RRMOVL || f_icode == CMOVL || f_icode == PUSHL || f_icode == POPL){
    f_valP = f_pc + 2;
  } else {
    f_valP = f_pc + 2;
  }
  return f_valP;
}

/* Function: predictPC
 * Description: Determines if the F_predPC needs to be set to f_valC or f_valP
 * Params: valP, valC, icode
 * Returns: none
 * Modifies: F_predPC    
 */
void predictPC(unsigned int valP, unsigned int valC, unsigned int icode){
  if(icode == JMP || icode == CALL){
    F.predPC = valC;
  } else {
    F.predPC = valP;
  }
}

/* Function: setPc
 * Description: Returns a copy of the F register
 * Params: f_newPC
 * Returns: none   
 * Modifies: F.predPC
 */
void setPc(unsigned int f_newPC)
{
  F.predPC = f_newPC;
}

/* Function: getStat
 * Description: Gets the stat based on the icode
 * Params: f_icode
 * Returns: f_stat     
 * Modifies: none        
 */
unsigned int getStat(unsigned int f_icode, bool memError, bool valid_icode)
{
  if(memError){
    return SADR;
  } else if(!valid_icode){
    return SINS;
  } else if(f_icode == HALT){
    return SHLT;
  } else {
    return SAOK;
  }
}

/* Function: getRA
 * Description: Returns the correct value for register A.
 * Params: f_pc
 * Returns: f_RA
 * Modifies: none
 */
unsigned int getRA(unsigned int f_pc, unsigned int f_icode, char regByte)
{
  if(f_icode == IRMOVL){
    return RNONE;
  } else {
    return getBits(4, 7, regByte);
  }
}

/* Function: getRB
 * Description: Returns the correct value for register B.
 * Params: f_pc
 * Returns: f_RB
 * Modifies: none                                   
 */ 
unsigned int getRB(unsigned int f_pc, unsigned int f_icode, char regByte)
{
  if(f_icode == IRMOVL || f_icode == OPL || f_icode == RRMOVL || f_icode == RMMOVL || f_icode == MRMOVL){
    return getBits(0, 3, regByte);
  } else if(f_icode == PUSHL || f_icode == POPL){
    return RESP;
  } else {
    return RNONE;
  }
}

/* Function: getValC
 * Description: Returns valC based on the icode and ifun, rearranges for 
                little endianness.
 * Params: f_pc, f_icode, f_ifun
 * Returns: f_valC
 * Modifies: none
 */
unsigned int getValC(unsigned int f_pc, unsigned int f_icode, unsigned int f_ifun, bool error)
{
  unsigned int f_valC;
  if(f_icode == DUMP || f_icode == JMP || f_icode == CALL){
    // Need to make VALC out of the four bytes that follow the first byte
    unsigned int byte0 = getByte((f_pc + 1), &error);
    unsigned int byte1 = getByte((f_pc + 2), &error);
    unsigned int byte2 = getByte((f_pc + 3), &error);
    unsigned int byte3 = getByte((f_pc + 4), &error);
    // Now we build a word out of the bytes using the buildWord function from tools.c
    f_valC = buildWord(byte0, byte1, byte2, byte3);
  } else if(f_icode == IRMOVL || f_icode == RMMOVL || f_icode == MRMOVL){
    unsigned int byte0 = getByte((f_pc + 2), &error);
    unsigned int byte1 = getByte((f_pc + 3), &error);
    unsigned int byte2 = getByte((f_pc + 4), &error);
    unsigned int byte3 = getByte((f_pc + 5), &error);
    f_valC = buildWord(byte0, byte1, byte2, byte3);
  } else {
    // Assume NOP or HALT for now
    f_valC = 0x0;
  }  
  return f_valC;
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
