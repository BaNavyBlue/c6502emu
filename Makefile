#OBJS specifies which files to compile as part of the project
MAIN = cM6502/main.cpp
SRC = 


OBJS = $(MAIN) $(SRC)
INCL = 

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -std=c++17 -w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS =

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = cpu

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(INCL) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

.PHONY : clean
