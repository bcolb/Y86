#ifndef FETCHSTAGE_H
#define FETCHSTAGE_H

typedef struct 
{
    unsigned int predPC;
} fregister;

void fetchStage(bool troubleshoot, forwardType forward, stallType stall);
bool getFStall(stallType stall);
bool getDStall(stallType stall);
bool getDBubble(stallType stall);
bool validInstruction(unsigned int f_icode);
bool needsRegIDs();
// unsigned int predictPC(unsigned int, unsigned int);
unsigned int selectPC(forwardType forward);
unsigned int pcIncrement(unsigned int f_icode, unsigned int f_pc, bool valid_opcode);
void predictPC(unsigned int valP, unsigned int valC, unsigned int icode);
void setPc(unsigned int newPC);
unsigned int getStat(unsigned int f_icode, bool memError, bool valid_icode);
unsigned int getRA(unsigned int f_pc, unsigned int f_icode, char regByte);
unsigned int getRB(unsigned int f_pc, unsigned int f_icode, char regByte);
unsigned int getValC(unsigned int f_pc, unsigned int f_icode, unsigned int f_ifun, bool error);
fregister getFregister();
void clearFregister();

#endif
