#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H
/*
 * struct to hold all the functioning units of the execute stage
 */
typedef struct 
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int valC;
    unsigned int valA;
    unsigned int valB;
    unsigned int dstE;
    unsigned int dstM;
    unsigned int srcA;
    unsigned int srcB;
} eregister;
/*
 * struct to hold function pointers
 */
typedef unsigned int (*functionptr)(void);

void executeStage(bool troubleshoot, forwardType *forward, statusType status, stallType *stall);
void initializeFuncPtrArray();
bool getMBubble(statusType status);
unsigned int performCall();
unsigned int performRet();
unsigned int performPushl();
unsigned int performPopl();
unsigned int performRrmovl();
unsigned int performRmmovl();
unsigned int performMrmovl();
unsigned int performCmov();
unsigned int performOpl(statusType status);
unsigned int performIrmovl();
unsigned int zeroFunc();
unsigned int dumpFunc();
bool getCnd();
unsigned int getEdstE(unsigned int e_cnd);
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int valC, unsigned int valA, unsigned int valB, unsigned int dstE, unsigned int dstM, unsigned int srcA, unsigned int srcB);
eregister getEregister();
void clearEregister();
#endif
