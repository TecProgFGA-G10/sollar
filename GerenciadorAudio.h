/*
* Solar 3D Técnicas de Programação
* Esse programa inicializa os sons e 
* toca os sons do jogo
*/

#ifndef GERENCIADOR_AUDIO_H
#define GERENCIADOR_AUDIO_H
#include <SDL/SDL_audio.h>
#include <SDL/SDL_mixer.h>
#include "SolarUtil.h"

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
int tocaMusica(Mix_Music *musica)
{
	if (musica == NULL) {
		printf("Unable to load Ogg file: %s\n", Mix_GetError());
		return 1;
	}
	else {
		/* nothing to do */
	}
	if (Mix_PlayMusic(musica, -1) == -1) {
		printf("Unable to play Ogg file: %s\n", Mix_GetError());
	}
	else {
		/* nothing to do */
	}
	return 0;
}

/* plays effects sound */
void PlaySound(int som,Mix_Chunk *somEfeito)
{
	if (som == MODELO_TIRO) {
		Mix_PlayChannel(2,somEfeito,0);
	}
	else if (som == MODELO_EXPLOSAO) {
		Mix_PlayChannel(3, somEfeito, 0);
	}

}

/* clear audio */
void limpa(Mix_Music *musica)
{
	Mix_HaltMusic();
	Mix_FreeMusic(musica);
	musica = NULL;
	Mix_HaltChannel(2);
	Mix_HaltChannel(3);
}

#endif
