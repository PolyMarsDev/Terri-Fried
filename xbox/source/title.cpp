#include "title.h"

#include "utils/input.h"
#include "utils/draw.h"
#include "utils/audio.h"

#include "game.h"
#include "score.h"

#include "textures/tex_logo.h"

const int screenWidth = 640;
const int screenHeight = 480;

uint32_t *logoSprite;

xbox_audioclip selectAudio;

void title_init() {
    resetScore();
    sprintf(highscore, "BEST: %d", highscoreInt);
    
    logoSprite = xbox_load_texture(tex_logo_rgba, 400, 100);
    
    selectAudio = xbox_audio_load_clip("D:\\media\\select.wav");
}

void title_run() {
    title_init();
    
    xbox_audio_play_clip(selectAudio);
    
    bool running = true;
    
    while(running) {
        input_poll();
        
        xbox_target_back_buffer();
        
        xbox_clear_screen(0xffeee4e1);
        
        if(input_button_down(SDL_CONTROLLER_BUTTON_START))
            break;
        
        xbox_draw_sprite(screenWidth/2 - 200, screenHeight/2 - 75, 400, 100, logoSprite, 400, 100);
        
        xbox_draw_text(highscore, screenWidth/2 - 41, screenHeight/2 + 10, 0.45f, 0x00, 0x00, 0x00);
        xbox_draw_text("PRESS A TO BEGIN", screenWidth/2 - 96, screenHeight/2 + 50, 0.4f, 0xd5, 0x80, 0x5a);
        
        if(input_button_down(SDL_CONTROLLER_BUTTON_A)) {
            xbox_audio_play_clip(selectAudio);
            running = false;
        }
        
        xbox_swap_buffers();
    }
    
    title_free();
}

void title_free() {
    if(selectAudio.sound) xbox_audio_free_clip(selectAudio);
    
    if(logoSprite) xbox_free_texture(logoSprite);
}
