#ifndef LOADER_H
#define LOADER_H
bool load(int argc, char *argv[]);
void grabBoth(char *line);

bool loadMemory(char *line);//from the checkline or something else
void loadInstruction();
bool validFileName(char *filename);
unsigned int getAddress(char *line);
void  getInstruction(char *line);
int getInstructionSize(char *line);
unsigned int grabDataByte(char *line, int start);
bool isSpaces(char *line, int start, int end);
bool checkHex(char *line, int start, int end);
bool checkLine(char *line);
bool checkForBar(char *line);
bool checkAddress(char *line);
bool checkInstruction(char *line);
bool checkOverwrite();
#endif
