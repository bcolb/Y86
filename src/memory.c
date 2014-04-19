#include "bool.h"
#include "memory.h"
#include "tools.h"

/* Global variables */
static unsigned int memory[MEMSIZE];

/* Function Declarations */
unsigned int fetch(int address, bool * memError);
void store(int address, unsigned int value, bool * memError);

/* Function: fetch
 * Description: Gets the value at the specified address, used internally
 *              in memory.c only.
 * Params: address, memError
 * Returns: unsigned int - the value
 * Modifies: none
 */
unsigned int fetch(int address, bool * memError)
{
  if(address > 1023 || address < 0) {
    *memError = TRUE; 
    return 0;
  } else {
    *memError = FALSE;
    return memory[address];
  }
}

/* Function: store
 * Description: Stores a value at the specified address, used internally
 *              only.
 * Params: address, value, memError
 * Returns: none
 * Modifies: memError
 */
void store(int address, unsigned int value, bool * memError)
{
  if(address > 1023 || address < 0) {
    *memError = TRUE;
  } else {
    *memError = FALSE;
    memory[address] = value;
  }
}

/* Function: getByte
 * Description: gets a byte value from a byte address
 * Params: address, memError
 * Returns: unsigned char (a single byte)
 * Modifies: memError
 */
unsigned char getByte(int address, bool * memError)
{
  if(address > 4095 || address < 0) {
    *memError = TRUE;
    return 0;
  } else {
    *memError = FALSE;
    int wordaddress = address / 4;
    unsigned int word = fetch(wordaddress, memError);
    int byteNumber = address % 4;
    unsigned char theByte = getByteNumber(byteNumber, word);
    return theByte;
  }
}

/* Function: putByte
 * Description: puts a byte at the specified address
 * Params: address, value, memError
 * Returns: none
 * Modifies: memError
 */
void putByte(int address, unsigned char value, bool * memError)
{
  if(address > 4095 || address < 0) {
    *memError = TRUE;
  } else {
    *memError = FALSE;
    int wordaddress = address / 4;
    unsigned int word = fetch(wordaddress, memError);
    int byteNumber = address % 4;
    unsigned int newWord = putByteNumber(byteNumber, value, word);
    store(wordaddress, newWord, memError);
  }
}

/* Function: clearMemory
 * Description: clears the memory buffer
 * Params: none
 * Returns: none
 * Modifies: memory
 */
void clearMemory()
{
  clearBuffer((char *) &memory, sizeof(memory));
}

/* Function: getWord
 * Description: Gets a word from a byte address
 * Params: address, memError
 * Returns: unsigned int (32 bit word)
 * Modifies: memError
 */
unsigned int getWord(int address, bool * memError)
{
  if(address > 4095 || address < 0 || address % 4 != 0) {
    *memError = TRUE;
    return 0;
  } else {
    *memError = FALSE;
    int wordaddress = address / 4;
    unsigned int word = fetch(wordaddress, memError);
    return word;
  }
}

/* Function: putWord
 * Description: Puts a word at the byte address
 * Params: address, value, memError
 * Returns: none
 * Modifies: memError
 */
void putWord(int address, unsigned int value, bool * memError)
{
  if(address > 4095 || address < 0 || address % 4 != 0) {
    *memError = TRUE;
  } else {
    *memError = FALSE;
    int wordaddress = address / 4;
    store(wordaddress, value, memError);
  }
}
