#include <stdio.h>
#include <stdlib.h>
#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "dump.h"
#include "stalling.h"
#include "status.h"
#include "forwarding.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"

void initialize();

/**
 * Creates and initializes variables for use in the YESS simulator.
 * Controls the running of the YESS simulator by first loading a program,
 * then proceeding through each stage in a loop until the program has 
 * completed run.
 * @param argc 
 * @param arg[v] the filename
 */
int main(int argc, char *argv[])
{
    // Boolean used for troubleshooting turns print statements on and dump off
    bool troubleshoot = FALSE;
    
    // Variables used with the loader
    bool memError;
    bool loadError;
    unsigned int memValue;
    // Variables used for logic associated with the main loop
    int clockCount;
    bool stop;
    // For forwarding, stalling, expections
    static stallType stall;
    static forwardType forward;
    static statusType status;

    // Clears the memory, general purpose registers and processor registers.
    //this should load AOK into blah, first set of zeros into PC valP
    initialize();

    if(!load(argc, argv)){
      dumpMemory();
      exit(0);
    }

    initializeFuncPtrArray();
    setPc(0x000);
    clockCount = 0;
    stop = FALSE;
    // Print for troubleshooting
    if(troubleshoot){
      printf("Starting Program\n\n");
    }
    // Main loop for the program, calls functions for each stage
    while (!stop){
      if(troubleshoot){
	printf("Cycle: %x\n", clockCount); 
	dumpProgramRegisters();
      }
      stop = writebackStage(troubleshoot, &forward, &status);
      memoryStage(troubleshoot, &forward, &status, &stall);
      executeStage(troubleshoot, &forward, status, &stall);
      decodeStage(troubleshoot, forward, &stall);
      fetchStage(troubleshoot, forward, stall);
      clockCount++;
      if(troubleshoot){
	printf("\n");
      }
    }    
    if(troubleshoot){
      dumpProgramRegisters();
      dumpProcessorRegisters();
      dumpMemory();
    }
    //dumpMemory();
    //dumpProgramRegisters();
    // Neccessary print statement prints off number of cycles
    printf("\nTotal clock cycles = %d\n", clockCount);
}

/**
 * Initializes the program by clearing the memory, general purpose registers
 * and processor registers.
 */
void initialize()
{
    clearMemory();
    clearRegisters();
    clearFregister();
    clearDregister();
    clearEregister();
    clearMregister();
    clearWregister();
}
