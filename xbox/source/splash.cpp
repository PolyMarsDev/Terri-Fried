#include "splash.h"

#include "utils/input.h"
#include "utils/draw.h"
#include "utils/audio.h"

#include "textures/tex_splash_egg.h"

const int screenWidth = 640;
const int screenHeight = 480;

uint32_t *splashEggSprite;

xbox_audioclip splashAudio;

void splash_init() {
    splashEggSprite = xbox_load_texture(tex_splash_egg_rgba, 32, 32);
    
    splashAudio = xbox_audio_load_clip("D:\\media\\splash.wav");
}

void splash_run() {
    splash_init();
    
    xbox_target_back_buffer();
    
    xbox_clear_screen(0xffeee4e1);
    
    xbox_audio_play_clip(splashAudio);
    
    xbox_draw_text("POLYMARS", screenWidth/2 - 59, screenHeight/2 + 3, 0.4f, 0xd5, 0x80, 0x5a);
    
    xbox_draw_sprite(screenWidth/2 - 16, screenHeight/2 - 16 - 23, 32, 32, splashEggSprite, 32, 32);
    
    xbox_swap_buffers();
    
    // Just show the splash for a few moments
    SDL_Delay(2500);
    
    splash_free();
}

void splash_free() {
    if(splashAudio.sound) xbox_audio_free_clip(splashAudio);
    
    if(splashEggSprite) xbox_free_texture(splashEggSprite);
}
