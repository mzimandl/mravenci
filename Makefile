#OBJS specifies which files to compile as part of the project
OBJS = \
src/main.cpp\
src/math.cpp\
src/settings.cpp\
src/scenario.cpp\
src/classes/cTimer.cpp\
src/classes/cTexture.cpp\
src/classes/cObject.cpp\
src/classes/cAnt.cpp\
src/classes/cColony.cpp\
src/classes/cPheromones.cpp\
src/classes/cSoundControl.cpp

#CC specifies which compiler we're using
CC = g++ -std=c++11

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = #-fopenmp #-w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = ants

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
