#ifndef MEMCHSTAGE_H
#define MEMSTAGE_H
/*
 * struct to hold all the unit variables during the memory stage
 */
typedef struct 
{
    unsigned int stat;
    unsigned int icode;
    unsigned int Cnd;
    unsigned int valE;
    unsigned int valA;
    unsigned int dstE;
    unsigned int dstM;
} mregister;

//prototypes for functions called from files other than Memory Stage
mregister getMregister();
bool getWStall(statusType *status);
unsigned int getMStat(bool mem_error);
unsigned int memRead(bool *mem_error);
unsigned int memWrite(bool *mem_error);
void clearMregister();
void memoryStage(bool troubleshoot, forwardType *forward, statusType *status, stallType *stall);
void updateMregister(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
#endif
