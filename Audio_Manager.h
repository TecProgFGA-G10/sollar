/*
* Solar 3D Técnicas de Programação
* Esse programa inicializa os sons e
* toca os sons do jogo
*/

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H
#include <SDL/SDL_audio.h>
#include <SDL/SDL_mixer.h>
#include "Solar_Utilities.h"

/* initializes the sound */
void iniciaAudio(int audio_rate,
				 Uint16 audio_format,
				 int audio_channels,
				 int audio_buffers)
{
	if (Mix_OpenAudio(audio_rate, audio_format,
					  audio_channels, audio_buffers) != 0)
	{
		printf("Unable to initialize audio: %s\n", Mix_GetError());
	}
	else {
		/* nothing to do */
	}
}

/* plays the music */
int tocaMusica(Mix_Music *music)
{
	if (music == NULL) {
		printf("Unable to load Ogg file: %s\n", Mix_GetError());
		return 1;
	}
	else {
		/* nothing to do */
	}
	if (Mix_PlayMusic(music, -1) == -1) {
		printf("Unable to play Ogg file: %s\n", Mix_GetError());
	}
	else {
		/* nothing to do */
	}
	return 0;
}

/* plays effects sound */
void PlaySound(int sound, Mix_Chunk *somEfeito)
{
	if (sound == SHOT_MODEL) {
		Mix_PlayChannel(2,somEfeito,0);
	}
	else if (sound == EXPLOSION_MODEL) {
		Mix_PlayChannel(3, somEfeito, 0);
	}

}

/* clear audio */
void limpa(Mix_Music *music)
{
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	music = NULL;
	Mix_HaltChannel(2);
	Mix_HaltChannel(3);
}

#endif
