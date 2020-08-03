#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utils/input.h"
#include "utils/draw.h"
#include "utils/audio.h"

#include "gameobjects/player.h"
#include "gameobjects/platform.h"

#include "score.h"

#include "textures/tex_coin.h"
#include "textures/tex_egg.h"
#include "textures/tex_lava.h"
#include "textures/tex_platform.h"
#include "textures/tex_scorebox.h"

Platform platforms[4] = {{0}, {1}, {2}, {3}};
Player player(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2, platforms[0].getY() - 32, 26, 32);

const int screenWidth = 640;
const int screenHeight = 480;

uint32_t *playerSprite;
uint32_t *lavaSprite;
uint32_t *platformSprite;
uint32_t *coinSprite;
uint32_t *scoreBoxSprite;

xbox_audioclip launchAudio;
xbox_audioclip dieAudio;
xbox_audioclip coinAudio;
xbox_audioclip clickAudio;

void resetGame() {
    resetScore();
    
    // Reset player and platforms to default positions
    for(int i = 0; i < 4; i++) {
        platforms[i].resetPosition(i);
    }
    player.velocityX = 0;
    player.velocityY = 0;
    
    player.x = platforms[0].getX() + platforms[0].getWidth()/2 - 26/2;
    player.y = platforms[0].getY() - player.height;
}

void checkPlayerCollision() {
    bool onPlatform = false;
    
    for(int i = 0; i < 4; i++) {
        // The player collided with the platform's coin
        if(platforms[i].getHasCoin() && player.x + player.width - 3 > platforms[i].getCoinX() && player.x + 3 < platforms[i].getCoinX() + 24 && player.y + player.height - 3 > platforms[i].getCoinY() && player.y + 3 < platforms[i].getCoinY() + 24) {
            addScore(1);
            platforms[i].setHasCoin(false);
            xbox_audio_play_clip(coinAudio);
        }
        // The player collided with the platform itself
        if(player.x + 1 < platforms[i].getX() + platforms[i].getWidth() && player.x + player.width > platforms[i].getX() && player.y + player.height >= platforms[i].getY() && player.y < platforms[i].getY() + platforms[i].getHeight()) {
            // Bottom of platform
            if(player.y > platforms[i].getY() + platforms[i].getHeight()/2) {
                player.velocityY = 5;
            }
            // Top of platform
            else if(player.y + player.height <  platforms[i].getY() + platforms[i].getHeight()) {
                onPlatform = true;
                player.y = platforms[i].getY() - player.height;
                player.y++;
            }
        }
    }
    
    player.isOnPlatform = onPlatform;
}

void game_init() {
    playerSprite    = xbox_load_texture(tex_egg_rgba, 32, 32);
    lavaSprite      = xbox_load_texture(tex_lava_rgba, 800, 96);
    platformSprite  = xbox_load_texture(tex_platform_rgba, 100, 32);
    coinSprite      = xbox_load_texture(tex_coin_rgba, 24, 24);
    scoreBoxSprite  = xbox_load_texture(tex_scorebox_rgba, 104, 72);
    
    launchAudio = xbox_audio_load_clip("D:\\media\\launch.wav");
    dieAudio    = xbox_audio_load_clip("D:\\media\\die.wav");
    coinAudio   = xbox_audio_load_clip("D:\\media\\coin.wav");
    clickAudio  = xbox_audio_load_clip("D:\\media\\click.wav");
}

void game_run() {
    game_init();
    
    int touchX = 0;
    int touchY = 0;
    
    int mouseDownX = 0;
    int mouseDownY = 0;
    
    int trajectoryArea = 300;

    double lavaY = screenHeight - 32;
    double lavaFloat = 0;
    
    bool firstTime = true;
    
    while(1) {
        input_poll();
        
        xbox_target_back_buffer();
        
        xbox_clear_screen(0xffeee4e1);
        
        if(input_button_down(SDL_CONTROLLER_BUTTON_START))
            break;
        
        touchX += input_axis(SDL_CONTROLLER_AXIS_LEFTX)/2000;
        touchY += input_axis(SDL_CONTROLLER_AXIS_LEFTY)/2000;
        
        // Limit player's trajectory movement to a certain area around them
        if(touchX > mouseDownX + trajectoryArea) touchX = mouseDownX + trajectoryArea;
        if(touchX < mouseDownX - trajectoryArea) touchX = mouseDownX - trajectoryArea;
        if(touchY > mouseDownY + trajectoryArea) touchY = mouseDownY + trajectoryArea;
        if(touchY < mouseDownY - trajectoryArea) touchY = mouseDownY - trajectoryArea;
        
        if(input_button_down(SDL_CONTROLLER_BUTTON_A) && player.isOnPlatform) {
            xbox_audio_play_clip(clickAudio);
            mouseDownX = touchX;
            mouseDownY = touchY;
        }
        
        int velocityX = touchX - mouseDownX;
        int velocityY = touchY - mouseDownY;
        
        if(input_button_release(SDL_CONTROLLER_BUTTON_A) && player.isOnPlatform) {
            if(firstTime) {
                firstTime = false;
            }
            else {
                xbox_audio_play_clip(launchAudio);
                if(player.isOnPlatform) {
                    player.y--;
                }
                
                player.velocityX = (double)velocityX*.08;
                player.velocityY = (double)velocityY*.08;
            }
        }
        
        checkPlayerCollision();
        player.updatePosition();
        if(player.y > screenHeight) {
            xbox_audio_play_clip(dieAudio);
            resetGame();
        }
        for(int i = 0; i < 4; i++) {
            platforms[i].updatePosition();
        }
        
        lavaY = screenHeight - 43 - (sin(lavaFloat) * 5);
        lavaFloat += 0.05;
        
        if(input_button_held(SDL_CONTROLLER_BUTTON_A) && player.isOnPlatform) {
            xbox_draw_trajectory(player.x + 16, player.y + 16, (double)velocityX*.08, (double)velocityY*.08);
        }
        
        for(int i = 0; i < 4; i++) {
            xbox_draw_sprite(platforms[i].getX(), platforms[i].getY(), 100, 32, platformSprite, 100, 32);
            if(platforms[i].getHasCoin()) {
                xbox_draw_sprite(platforms[i].getCoinX(), platforms[i].getCoinY(), 24, 24, coinSprite, 24, 24);
            }
        }
        
        xbox_draw_sprite(player.x, player.y, 32, 32, playerSprite, 32, 32);
        xbox_draw_sprite(0, lavaY, 800, 96, lavaSprite, 800, 96);
        xbox_draw_sprite(17, 17, 104, 72, scoreBoxSprite, 104, 72);
        
        xbox_draw_text(score, 28 - 2, 29, 0.8f, 0x00, 0x00, 0x00);
        xbox_draw_text(highscore, 17, 90, 0.4f, 0x00, 0x00, 0x00);
        
        xbox_swap_buffers();
    }
    
    game_free();
}

void game_free() {
    if(launchAudio.sound) xbox_audio_free_clip(launchAudio);
    if(dieAudio.sound)    xbox_audio_free_clip(dieAudio);
    if(coinAudio.sound)   xbox_audio_free_clip(coinAudio);
    if(clickAudio.sound)  xbox_audio_free_clip(clickAudio);
    
    if(playerSprite)   xbox_free_texture(playerSprite);
    if(lavaSprite)     xbox_free_texture(lavaSprite);
    if(platformSprite) xbox_free_texture(platformSprite);
    if(coinSprite)     xbox_free_texture(coinSprite);
    if(scoreBoxSprite) xbox_free_texture(scoreBoxSprite);
}
