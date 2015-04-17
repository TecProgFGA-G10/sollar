all:
	gcc -Wall Game_Ship_Main.c glm.c -o exec -lGL -lglut -lGLU -lSDL -lm -lSDL_mixer
	
compile:
	optirun ./exec
