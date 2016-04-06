
all : main
#include givenFiles
main: main.c givenFiles/GL_utilities.c
	cc -Wall -std=c99 -o main -framework OpenGL -framework cocoa -lm -DGL_GLEXT_PROTOTYPES main.c world.c givenFiles/LoadTGA.c givenFiles/VectorUtils3.c givenFiles/loadobj.c givenFiles/GL_utilities.c givenFiles/MicroGlut.m

clean :
	rm main


