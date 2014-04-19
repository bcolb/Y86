#stripped down make by Rudy Garceau
#the c compiler

CC	= gcc
CFLAGS	= -g
LD	= $(CC)

#the a.out = yess
EXE	= yess

#one shot for all .c files
SRCS	= decodeStage.c dump.c executeStage.c fetchStage.c main.c memory.c memoryStage.c registers.c tools.c writebackStage.c loader.c

#now, tricky way to convert one to one dependencies
OBJS	= ${SRCS:.c=.o}
all : $(EXE)

#the exe depends on the OBJ's- converted .o files
$(EXE) : $(OBJS)
	$(LD) -o $@ $(OBJS)

#the .o files depend on the .h files
$(OBJS) :  decodeStage.h dump.h executeStage.h fetchStage.h memory.h memoryStage.h registers.h tools.h writebackStage.h loader.h instructions.h

#force wipeout all .o and exe
clean :
	rm -f *.o yess
