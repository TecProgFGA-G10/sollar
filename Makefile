all:
	gcc -Wall jogoNaveMain.c glm.c -o exec -lGL -lglut -lGLU -lSDL -lm -lSDL_mixer
	
compile:
	optirun ./exec
