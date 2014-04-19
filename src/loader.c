#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include "bool.h"
#include "memory.h"
#include "loader.h"

// Static variables used to keep track of current and past instructions
static unsigned int curAd;
static char* curIn;
static unsigned int curSize;
static unsigned int lastAd;
static unsigned int lastSize;

/* Function: load
 * Description: Controls the flow of the load program, loads program into memory.
 * Params: argc, *argv[]
 * Returns: boolean value
 * Modifies: none
 */
bool load(int argc, char *argv[])
{
  if(validFileName(argv[1]))
    {
      FILE *fp;
      fp = fopen((argv[1]), "r");
      char buf[160];
      int count = 0; // Keeps count of the line
      // Processing file line by line
      while(fgets(buf, sizeof buf, fp) != NULL)
        {
	  // Print statements for troubleshooting
	  // printf("Line %x\n", count);
	  // printf("%s\n", buf);
          if(!checkLine(buf)) 
            {
              printf("Error on line %d\n", count + 1);
              printf("%s\n", buf);
              fclose(fp);
              return  FALSE; // Exits the program, returns FALSE for error
            }
	  // Updating the current address and instruction
	  curIn = malloc(8);
	  curAd = getAddress(buf);
	  curSize = getInstructionSize(buf);
	  getInstruction(buf);
	  // Actual check for overwrite error
	  if(!checkOverwrite()){
	    printf("Error on line %d\n", count + 1);
	    int num = strlen(buf);
	    if(num >= 80){
	      printf("%s", buf);
	    } else {
	      printf("%s\n", buf);
	    }
	    fclose(fp);
	    return FALSE;
	  }
	  loadInstruction();
	  if(curSize > 0){
	    lastAd = getAddress(buf);
	    lastSize = getInstructionSize(buf);
	  }
	  count++;
        }
    } else {
    printf("File opening failed.\n");
    printf("usage: yess <filename>.yo\n");
    return FALSE; // Exits program, returns FALSE for error
  }
  return TRUE; // Exits program - returns TRUE - no error
}

/* Function: loadInstruction
 * Description: Loads a single instruction into memory.
 * Params: none - but uses static variables curIn and curAd
 * Returns: none
 * Modifies: Location in memory
 */
void loadInstruction()
{
  bool error;
  if(curSize > 0){
    int counter = 0;
    for(counter = 0; counter < curSize; counter++){
      unsigned int theByte = grabDataByte(curIn, counter*2);
      unsigned char value = (unsigned char) theByte;
      putByte(curAd, value, &error);
      curAd++;
    }
  }
}

/* Function: validFileName
 * Description: Checks for a valid file name.
 * Params: filename
 * Returns: bool, true if valid, false if invalid
 * Modifies: none
 */
bool validFileName(char *filename)
{
  if(filename == NULL) // Checks for a Null filename
    {
      return FALSE;
    }
  char * extension;
  extension = strstr(filename, ".yo"); // Checks for a .yo filename
  if(extension == NULL)
    {
      return FALSE;
    }
  FILE *fp;
  if (fp = fopen((filename), "r")) // Check to see if the file exists
    {
      fclose(fp);
      return TRUE;
    }
  return FALSE;
}

/* Function: checkOverwrite
 * Description: Checks for overwrite between instructions
 * Params: none - but uses static variables lastSize, lastAd, and curAd
 * Returns: bool - True if valid, False if invalid
 * Modifies: none
 */
bool checkOverwrite()
{
  if(lastSize == 0){
    return TRUE;
  } else if (curAd == 0xffffffff || lastAd == 0xffffffff) {
    return TRUE;
  } else if ((lastAd + lastSize) <= curAd){
    return TRUE;
  } else {
    return FALSE;
  }
}

/* Function: getAddress
 * Description: Extracts the memory address from a given line
 * Params: line
 * Returns: unsigned int containing address
 * Modifies: none
 */
unsigned int getAddress(char *line)
{
  if(isspace(line[2])){
    return -1;
  }
  char theaddress[3];
  theaddress[0] = line[4];
  theaddress[1] = line[5];
  theaddress[2] = line[6];
  unsigned int hexaddress;
  sscanf(theaddress, "%x", &hexaddress);
  return hexaddress;
}

/* Function: getInstruction
 * Description: Extracts the instruction from the provided line.
 * Params: line
 * Returns: none
 * Modifies: curIn
 */
void getInstruction(char *line){
  char* theInstruction = malloc(8);
  int i;
  int j = 0;
  for(i = 9; i < 21; i++){
    if(isspace(line[i])){
      return;
    } else {
      curIn[j] = line[i];
      j++;
    }
  }
}

/* Function: getInstructionSize
 * Description: Gets the size of the instruction in the line passed.
 * Params: line
 * Returns: int (instruction size)
 * Modifies: none
 */
int getInstructionSize(char *line)
{
  int index;
  int length = 0;
  for(index = 9; index < 21; index++){
    if(isspace(line[index])){
      return length/2;
    } else {
      length++;
    }
  }
  return length/2;
}

/* Function: grabDataByte
 * Description: Grabs a byte from a provided line.
 * Params: line, start
 * Returns: unsigned int containing the byte
 * Modifies: none
 */
unsigned int grabDataByte(char *line, int start)
{
  char byte[2];
  byte[0] = line[start];
  byte[1] = line[start + 1];
  unsigned int theByte;
  sscanf(byte, "%x", &theByte);
  return theByte;
}

/* Function: isSpaces
 * Description: Checks to see if the line contains just spaces in a given portion of the line.
 * Params: line, start, end
 * Returns: bool - True if is all spaces, false if contains char other than space
 * Modifies: none
 */
bool isSpaces(char *line, int start, int end)
{
  int i;
  for(i = start; i < end; i++){
    if(!isspace(line[i])){
      return FALSE;
    }
  }
  return TRUE;
}

/* Function: checkHex
 * Description: Checks to see if characters in the provided range are hexadecimal.
 * Params: line, start, end
 * Returns: bool - true if is all hex, false if contains char other than hex
 * modifies: none
 */
bool checkHex(char *line, int start, int end)
{
  int i;
  for(i = start; i < end; i++){
    if(!isxdigit(line[i])){
      return FALSE;
    }
  } // end outer for loop
    return TRUE;
}

/* Function: checkLine
 * Description: Contains control logic to perform all checks on a line except check overwrite which is called in load.
 * Params: line
 * Returns: bool - True if line is valid, False otherwise
 * Modifies: none
 */
bool checkLine(char *line)
{
  if(checkForBar(line)){
    char *firstpart = line;
    // Checks to see if the first part is all spaces, returns true if it is.
    if(isSpaces(firstpart, 0, 21)){
      return TRUE;
    }
    // Checks to see if there are spaces in the right spots
    if(!(firstpart[0] == ' ' && firstpart[1] == ' ' && firstpart[8] == ' ' && firstpart[21] == ' ')){
      // printf("Error Thrown - Case Spaces are not in the right spots\n");
      return FALSE;
    }
    // Checks to see if there is a valid address followed by spaces (No Instruction)
    if(checkAddress(firstpart) && isSpaces(firstpart, 9, 21)){
      return TRUE;
    }
    // Check Instruction and Check Address
    if(checkAddress(firstpart) && checkInstruction(firstpart)){
      return TRUE;
    }
  } else {
    // printf("Error thrown - other case\n");
    return FALSE;
  }
}

/* Functin: checkForBar
 * Description: Checks for a bar at character 22 in the line.
 * Params: line
 * Returns: Bool - True if passed - False otherwise
 * Modifies: none
 */
bool checkForBar(char *line)
{
  if(line[22] == '|'){
    return TRUE;
  }else{
    // printf("Check for bar failed\n");
    return FALSE;
  }
}

/* Function: checkAdress
 * Description: Checks to make sure the address is valid in a provided line.
 * Params: line
 * Returns: bool True if valid, False otherwise
 * Modifies: none
 */
bool checkAddress(char *line)
{
  // Check that each address follows the format of 0x followed by three hex digits
  if(line[2] == '0' && line[3] == 'x' && checkHex(line, 4, 6) && line[7] == ':'){
    return TRUE;
  } else {
    return FALSE;
  }
}

/* Function: checkInstruction
 * Description: Checks for valid instruction length in a given line
 * Params: line
 * Returns: bool - True if valid - False otherwise
 * Modifies: none
 */
bool checkInstruction(char *line)
{
  int i = 0;
  for(i = 9; i < 21; i++){
    if(line[i] == ' '){
      if(!isSpaces(line, i, 21)){
	return FALSE;
      } else if (i % 2 != 1){
	return FALSE;
      } else {
	return TRUE;
      }
    } else {
      if(!isxdigit(line[i])){
	return FALSE;
      }
    }
  }
  return TRUE;
}
