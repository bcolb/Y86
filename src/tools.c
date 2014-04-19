/**
 * Tools.c
 *
 * Practice functions for CS 3481 dealing with bit manipulation.
 *
 * @author Brice Colbert
 * @version Jan 21, 2013
 */
#include <stdio.h>
#include "bool.h"
#include "tools.h"

/* 
 * Function: getBits
 * Description: takes a source and returns the appriopriate bits from hi to low
 * Params: int low, int high, unsigned int source
 * Returns: unsigned int or zero if out of bounds
 * Modifies: nothing
 */
unsigned int getBits(int low, int high, unsigned int source)
{
  unsigned int num;
  if ((low < 0) || (low > 31) || (high < 0) || (high > 31) || (low > high))
    {
      return 0;
    }
  else
    {
      num = source << (31 - high);
      num = num >> (31 - high + low);
      return num;
    }
} 

/* 
 * Function: setBits
 * Description: takes a source and sets the appriopriate bits from lo to hi
 * Params: int low, int high, unsigned int source
 * Returns: unsigned int or 0 if out of bounds
 * Modifies: nothing
 */
unsigned int setBits(int low, int high, unsigned int source)
{
  unsigned int num;
  if ((low < 0) || (low > 31) || (high < 0) || (high > 31) || (low > high))
    {
      return source;
    }
  else
    {
      num = 0xFFFFFFFF;
      num = num >> low;
      num = num << (31 - high + low);
      num = num >> (31 - high);
      return (num | source);
    }
}

/* 
 * Function: clearBits
 * Description: takes a source and clears the appriopriate bits from lo to hi
 * Params: int low, int high, unsigned int source
 * Returns: unsigned int or unchanged source if out of bounds
 * Modifies: nothing
 */
unsigned int clearBits(int low, int high, unsigned int source)
{
  unsigned int num;
  if ((low < 0) || (low > 31) || (high < 0) || (high > 31) || (low > high))
    {
      return source;
    }
  else
    {
      num = 0xFFFFFFFF;
      num = num >> low;
      num = num << (31 - high + low);
      num = num >> (31 - high);
      return (~num & source);
    }
}

/* 
 * Function: assignOneBit
 * Description: takes a source and sets one bit
 * Params: int bitNumber, int bitValue, unsigned int source
 * Returns: unsigned int or unchanged source if out of bounds
 * Modifies: nothing
 */
unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source)
{
  unsigned int num;
  if ((bitNumber < 0) || (bitNumber > 31) || (bitValue < 0) || (bitValue > 1))
    {
      return source;
    }
  else
    {
      num = 0xFFFFFFFF;
      num = num >> bitNumber;
      num = num << 31;
      num = num >> (31 - bitNumber);
      if(bitValue == 1) 
	{
	num = source | num;
	} 
      else 
	{
	num = ~num;
	num = source & num;
	}
      return num;
    }
}

/* 
 * Function: getBytenumber
 * Description: returns a particular byte of information from source
 * Params: int byteNo, unsigned int source
 * Returns: unsigned char or 0 if out of bounds
 * Modifies: nothing
 */
unsigned char getByteNumber(int byteNo, unsigned int source)
{
  if((byteNo < 0) | (byteNo > 3)) 
    {
      return 0;
    }
  else
    {
      unsigned int num;
      num = source >> (8 * byteNo);
      num = num & 0x000000ff;
      return (char) num;
    }
}

/* 
 * Function: putBytenumber
 * Description: places a particular byte of information into source
 * Params: int byteNo, unsigned char byteValue, unsigned int source
 * Returns: unsigned int or unchanged source if out of bounds
 * Modifies: nothing
 */
unsigned int putByteNumber(int byteNo, unsigned char byteValue, 
                           unsigned int source)
{
  if((byteNo < 0) | (byteNo > 3))
    {
      return source;
    }
  else
    {
      unsigned int num;
      unsigned int checknum;
      unsigned int answer;
      checknum = 0x000000FF << (byteNo * 8);
      num = (unsigned int) byteValue;
      num = num << (byteNo * 8);
      answer = source;
      answer = answer & ~checknum;
      answer = answer | num;
      return answer;
    }
}

/* 
 * Function: buildWord
 * Description: makes a unsigned int out of 4 unsigned chars
 * Params: four unsigned chars 
 * Returns: the built unsigned int 
 * Modifies: nothing
 */
unsigned int buildWord(unsigned char byte0, unsigned char byte1, 
                       unsigned char byte2, unsigned char byte3)
{
  unsigned int num;
  num = 0x00000000;
  num = (num | (unsigned int) byte3);
  num = num << 8;
  num = (num | (unsigned int) byte2);
  num = num << 8;
  num = (num | (unsigned int) byte1);
  num = num << 8;
  num = (num | (unsigned int) byte0);
  return num;
}

/* 
 * Function: isNegative
 * Description: Returns 1 if the source value would be negative if interpreted as a signed integer.
 * Params: unsigned int source
 * Returns: boolean
 * Modifies: nothing
 */
bool isNegative(unsigned int source)
{
  unsigned int num;
  num = source >> 31;
  if(num == 1)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

/* 
 * Function: expandBits
 * Description: Takes an unsigned int source and array of length 36 and returns the array
 * Params: unsigned int sourcei, char bits[]
 * Returns: nothing
 * Modifies: bits[]
 */
void expandBits(unsigned int source, char bits[36])
{
  unsigned int tempNum;
  int i = 0;
  int sourcePos = 31;
  for(i=0; i < 36; i++)
    {
      if((i == 8) || (i == 17) || (i == 26))
	{
	  bits[i] = (char) ' ';
	}
      else if(i == 35)
	{
	  bits[i] = 0;
	}
      else
	{
	  tempNum = source >> sourcePos;
	  if((tempNum % 2) == 1)
	    {
	      bits[i] = (char) '1';
	    }
	  else
	    {
	      bits[i] = (char) '0';
	    }
	  sourcePos = sourcePos - 1;
	}
    }
}


/* 
 * Function: clearBuffer
 * Description: Clears an item by taking the data a pointer points to and setting it to zero
 * Params: char *buff, int size
 * Returns: nothing
 * Modifies: buff
 */
void clearBuffer(char * buff, int size)
{
  int i = 0;
  for(i = 0; i < size; i++)
    {
      *buff = 0;
      *buff++;
    }
  return;
}

