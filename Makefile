all:
	gcc -Wall Game_Ship_Main.c glm.c -o sollar -lGL -lglut -lGLU -lSDL -lm -lSDL_mixer -std=c99 -pedantic-errors

compile:
	optirun ./sollar
