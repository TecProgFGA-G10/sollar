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
#include <stdio.h>
#include "logger.h"

/* initializes the sound */
void audio_initialize(int audio_rate,
				 Uint16 audio_format,
				 int audio_channels,
				 int audio_buffers)
{
	if (Mix_OpenAudio(audio_rate, audio_format,
					  audio_channels, audio_buffers) == 0) {
		print_verbose_log("Audio sucessfully initiated");
	}
	else {
		print_error_log("Unable to initialize audio: %s", Mix_GetError());
	}
}

/* plays the music */
int play_music(Mix_Music *music)
{
	int return_value = 0; /* if everything is okay, return 0*/
	if (music == NULL) {
		print_verbose_log("Music sucessfully initiated");
	}
	else if (Mix_PlayMusic(music, -1) == -1) {
		print_error_log("Unable to play Ogg file: %s", Mix_GetError());
		return_value = 1;
	}
	else {
		print_error_log("Unable to load Ogg file: %s", Mix_GetError());
		return_value = 1;
	}
	return return_value;
}

/* plays effects sound */
void play_sound(int sound, Mix_Chunk *sound_effect)
{
	if (sound == SHOT_MODEL) {
		Mix_PlayChannel(2, sound_effect, 0);
		print_verbose_log("Shot audio effect");
	}
	else if (sound == EXPLOSION_MODEL) {
		Mix_PlayChannel(3, sound_effect, 0);
		print_verbose_log("Explosion audio effect");
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
	print_verbose_log("Audio cleared");
}

#endif
