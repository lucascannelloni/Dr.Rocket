
all : main

main: main.c GL_utilities.c
	cc -Wall -std=c99 -o main -framework OpenGL -framework cocoa -lm -DGL_GLEXT_PROTOTYPES main.c LoadTGA.c VectorUtils3.c loadobj.c GL_utilities.c MicroGlut.m

clean :
	rm main


