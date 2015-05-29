all:
	clang -w -Wall Game_Ship_Main.c logger.c glm.c -o sollar -lGL -lglut -lGLU -lSDL -lm -lSDL_mixer -std=c11 -pedantic-errors

compile:
	optirun ./sollar
