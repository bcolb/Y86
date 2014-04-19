//declarations for all the variable names
#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#define NOP 0x1//0
#define HALT 0x0//0
#define RRMOVL 0x2//0
#define CMOVLE 0x2//1
#define CMOVL 0x2//2
#define CMOVE 0x2//3
#define CMOVNE 0x2//4
#define CMOVGE 0x2//5
#define CMOVG 0x2//6
#define IRMOVL 0x3//0
#define RMMOVL 0x4//0
#define MRMOVL 0x5//0
#define OPL 0x6//x
#define ADDL 0x6//0
#define SUBL 0x6//1
#define ANDL 0x6//2
#define XORL 0x6//3
#define JMP 0x7//0
#define JLE 0x7//1
#define JL 0x7//2
#define JE 0x7//3
#define JNE 0x7//4
#define JGE 0x7//5
#define JG 0x7//6
#define CALL 0x8//0
#define RET 0x9//0
#define PUSHL 0xa//0
#define POPL 0xB//0
#define DUMP 0xC//0
#define FNONE 0x0
#define RESP 0x4
#define RNONE 0xF
#define ALUADD 0x0
#define SAOK 0x1
#define SHLT 0x2
#define SADR 0x3
#define SINS 0x4
#endif
