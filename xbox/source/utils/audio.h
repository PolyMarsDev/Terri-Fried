#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdint.h>
#include <SDL.h>

typedef struct {
    SDL_AudioSpec spec;
    Uint8 *sound;       // Pointer to wave data
    Uint32 soundlen;    // Length of wave data
} xbox_audioclip;

void xbox_audio_init();

xbox_audioclip xbox_audio_load_clip(const char *filename);

void xbox_audio_play_clip(xbox_audioclip clip);

void xbox_audio_free_clip(xbox_audioclip clip);

void xbox_audio_free();

#endif
