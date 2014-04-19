#ifndef DECODESTAGE_H
#define DECODESTAGE_H
/*
 * struct to hold all functional unit variables for the decode stage
 */
typedef struct  
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA;
    unsigned int rB;
    unsigned int valC;
    unsigned int valP;
} dregister;

//prototypes for functions called from files other than decode Stage
void decodeStage(bool troubleshoot, forwardType forward, stallType *stall);
bool getEBubble(stallType *stall);
unsigned int getDrvalA(unsigned int d_srcA);
unsigned int getDrvalB(unsigned int d_srcB);
unsigned int getSrcA();
unsigned int getSrcB();
unsigned int getDstE();
unsigned int getDstM();
unsigned int selectFwdA(unsigned int d_srcA, unsigned int d_rvalA, forwardType forward);
unsigned int forwardB(unsigned int d_srcB, unsigned int d_rvalB, forwardType forward);
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP);
dregister getDregister();
void clearDregister();

#endif
