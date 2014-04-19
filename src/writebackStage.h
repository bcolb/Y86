#ifndef WRITEBACKSTAGE_H
#define WRITEBACKSTAGE_H
/*
 * struct to hold all unit variables for the writeback stage
 */
typedef struct 
{
    unsigned int stat;
    unsigned int icode;
    unsigned int valE;
    unsigned int valM;
    unsigned int dstE;
    unsigned int dstM;
} wregister;

//prototypes for functions called from files other than writebackStage
void updateWregister(unsigned int stat, unsigned int icode, unsigned int valE, unsigned int valM, unsigned int dstE, unsigned int dstM);
void performDump();
wregister getWregister();
void clearWregister();
bool writebackStage(bool troubleshoot, forwardType *forward, statusType *status);
#endif
