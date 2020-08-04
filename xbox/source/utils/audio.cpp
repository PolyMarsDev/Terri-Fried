#include "audio.h"

// BUG: Disabling all audio code due to severe random issues in-game when using it,
// ranging from hang on startup, to lack of audio, to drifting joysticks
// (Can be one, a mix or all of the above symptoms during any running instance)

void xbox_audio_init() {
    //SDL_InitSubSystem(SDL_INIT_AUDIO);
}

xbox_audioclip xbox_audio_load_clip(const char *filename) {
    xbox_audioclip out;
    //SDL_LoadWAV(filename, &out.spec, &out.sound, &out.soundlen);
    
    //out.spec.callback = NULL; // Push the audio data instead
    
    return out;
}

void xbox_audio_play_clip(xbox_audioclip clip) {
    //SDL_OpenAudio(&clip.spec, NULL);
    
    //SDL_PauseAudio(0);
    //SDL_QueueAudio(1, clip.sound, clip.soundlen);
}

void xbox_audio_free_clip(xbox_audioclip clip) {
    //SDL_FreeWAV(clip.sound);
}

void xbox_audio_free() {
    //SDL_CloseAudio();
    //SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
