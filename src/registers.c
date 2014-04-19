#include "bool.h"
#include "registers.h"
#include "instructions.h"

/* Global Variables */
static unsigned int registers[REGSIZE];
static unsigned int cc;

/* Function: getRegister
 * Description: Gets the unsigned int currently in the specified register
 * Params: regNum
 * Returns: unsigned int - the register value
 * Modifies: none
 */
unsigned int getRegister(int regNum)
{
  return registers[regNum];
}

/* Function: setRegister
 * Description: Sets the register at regNum to the provided value, regValue.
 * Params: regNum, regValue
 * Returns: none
 * Modifies: registers
 */
void setRegister(int regNum, unsigned int regValue)
{
  registers[regNum] = regValue;
}

/* Function: clearRegisters
 * Description: Clears the condition codes and buffer for the registers.
 * Params: none
 * Returns: none
 * Modifies: registers, cc
 */
void clearRegisters()
{
  clearBuffer((char *) &registers, sizeof(registers));
  cc = clearBits(0, 31, cc);
}

/* Function: setCC
 * Description: Sets the condition code.
 * Params: bitNumber, value
 * Returns: none
 * Modifies: cc
 */
void setCC(unsigned int bitNumber, unsigned int value)
{
  cc = assignOneBit(bitNumber, value, cc);
}

/* Function: getCC
 * Description: gets the value of the specified bit in CC.
 * Params: bitNumber
 * Returns: unsigned int - the bit value
 * Modifies: none
 */
unsigned int getCC(unsigned int bitNumber)
{
  return getBits(bitNumber, bitNumber, cc);
}
