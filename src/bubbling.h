#ifndef BUBBLING_H
#define BUBBLING_H
/*
 * bubbleType is the way in which each stage
 * is signalled to bubble appropriate fields
 */
typedef struct
{
  unsigned int d_srcA;
  unsigned int d_srcB;
  unsigned int E_dstM;
  unsigned int e_Cnd;
  unsigned int E_icode;
  unsigned int D_icode;
  unsigned int M_icode;
} bubbleType;

#endif
