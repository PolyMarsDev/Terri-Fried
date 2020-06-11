#include <math.h>
#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "player.h"
#include "platform.h"

const double pi = 3.1415926535897;
const int gravity = 1;

Platform platforms[4] = {{0}, {1}, {2}, {3}};
Player player(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2, platforms[0].getY() - player.getHeight(), 26, 32);

int scoreInt = 0;
int highscoreInt = 0; //LoadStorageValue(0); // TODO: Implement natively
char score[32];
char highscore[32];

bool titleScreen = true;
bool playCoinFX = false;

void addScore(int amount) {
    scoreInt += amount;
    
    if (scoreInt < 10)
        sprintf(score, "00%d", scoreInt);
    else if (scoreInt < 100)
        sprintf(score, "0%d", scoreInt);
    else
        sprintf(score, "%d", scoreInt);
    
    if (scoreInt > highscoreInt) {
        highscoreInt = scoreInt;
        sprintf(highscore, "BEST: %d", highscoreInt);
    }
}

void resetScore() {
    scoreInt = 0;
    sprintf(score, "00%d", scoreInt);
    // SaveStorageValue(0, highscoreInt); // TODO: Implement natively
}

void resetGame() {
    resetScore();
    
    for (int i = 0; i < 4; i++)
        platforms[i] = Platform(i);
    
    player.setVelocity(0, 0);
    player.setX(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2);
    player.setY(platforms[0].getY() - player.getHeight());
}


void checkPlayerCollision() {
    bool onPlatform = false;
    
    for (int i = 0; i < 4; i++) {
        if (platforms[i].getHasCoin() && player.getX() + player.getWidth() - 3 > platforms[i].getCoinX() && player.getX() + 3 < platforms[i].getCoinX() + 24 && player.getY() + player.getHeight() - 3 > platforms[i].getCoinY() && player.getY() + 3 < platforms[i].getCoinY() + 24) {
            addScore(1);
            platforms[i].setHasCoin(false);
            playCoinFX = true;
        }
        
        if (player.getX() + 1 < platforms[i].getX() + platforms[i].getWidth() && player.getX() + player.getWidth() > platforms[i].getX() && player.getY() + player.getHeight() >= platforms[i].getY() && player.getY() < platforms[i].getY() + platforms[i].getHeight()) {
            if (player.getY() > platforms[i].getY() + platforms[i].getHeight()/2) {
                player.setVelocity(player.getVelocity().x, 5);
            } 
            else if (player.getY() + player.getHeight() <  platforms[i].getY() + platforms[i].getHeight()) {    
                onPlatform = true;
                player.setY(platforms[i].getY() - player.getHeight());
                player.setY(player.getY() + 1);
            }
        }
    }
    player.setOnPlatform(onPlatform);
}


int main(int argc, char **argv) {
    srand(time(NULL));
    
    resetScore();
    sprintf(highscore, "BEST: %d", highscoreInt);
    
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    int mouseDownX = 0;
    int mouseDownY = 0;
    
    double lavaY = screenHeight - 32;
    double timer = 0; 
    double splashTimer = 0;
    
    bool firstTime = true;
    bool playedSplash = false;
    bool playedSelect = false;
    
    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    
    window = SDL_CreateWindow(
        "Terri-Fried",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screenWidth, screenHeight,
        SDL_WINDOW_OPENGL
    );
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    
    SDL_Surface* egg = IMG_Load("resources/egg.png");
    SDL_SetWindowIcon(window, egg);
    
    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096);
    // SetMasterVolume(0.3f);
    
    SDL_Surface* playerSprite_surf = IMG_Load("resources/egg.png");
    SDL_Surface* lavaSprite_surf = IMG_Load("resources/lava.png");
    SDL_Surface* platformSprite_surf = IMG_Load("resources/platform.png");
    SDL_Surface* coinSprite_surf = IMG_Load("resources/coin.png");
    SDL_Surface* scoreBoxSprite_surf = IMG_Load("resources/scorebox.png");
    SDL_Surface* logo_surf = IMG_Load("resources/logo.png");
    SDL_Surface* splashEggSprite_surf = IMG_Load("resources/splash_egg.png");
    
    SDL_Texture* playerSprite = SDL_CreateTextureFromSurface(renderer, playerSprite_surf);
    SDL_Texture* lavaSprite = SDL_CreateTextureFromSurface(renderer, lavaSprite_surf);
    SDL_Texture* platformSprite = SDL_CreateTextureFromSurface(renderer, platformSprite_surf);
    SDL_Texture* coinSprite = SDL_CreateTextureFromSurface(renderer, coinSprite_surf);
    SDL_Texture* scoreBoxSprite = SDL_CreateTextureFromSurface(renderer, scoreBoxSprite_surf);
    SDL_Texture* logo = SDL_CreateTextureFromSurface(renderer, logo_surf);
    SDL_Texture* splashEggSprite = SDL_CreateTextureFromSurface(renderer, splashEggSprite_surf);
    
    Mix_Chunk* fxLaunch = Mix_LoadWAV("resources/launch.wav"); 
    Mix_Chunk* fxClick = Mix_LoadWAV("resources/click.wav"); 
    Mix_Chunk* fxDeath = Mix_LoadWAV("resources/die.wav"); 
    Mix_Chunk* fxCoin = Mix_LoadWAV("resources/coin.wav"); 
    Mix_Chunk* fxSplash = Mix_LoadWAV("resources/splash.wav");
    Mix_Chunk* fxSelect = Mix_LoadWAV("resources/select.wav");
    
    TTF_Font* font = TTF_OpenFont("resources/font.otf", 64);
    
    bool quit = false;
    
    bool mouse_pressed = false;
    
    int mouse_x, mouse_y;
    
    while (!quit) {
        SDL_Event e;
        bool mouse_released = false;
        bool mouse_down = false;
        
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT: {
                    quit = true;
                } break;
                case SDL_MOUSEBUTTONDOWN: {
                    mouse_pressed = true;
                    mouse_down = true;
                } break;
                case SDL_MOUSEBUTTONUP: {
                    mouse_pressed = false;
                    mouse_released = true;
                } break;
            }
        }
        
        SDL_PumpEvents();
        SDL_GetMouseState(&mouse_x, &mouse_y);
        
        SDL_Delay(16);
        
        if (titleScreen) {
            if (splashTimer > 120) {
                if (!playedSelect) {
                    Mix_PlayChannel(-1, fxSelect, 0);
                    playedSelect = true;
                }
                
                SDL_SetRenderDrawColor(renderer, 0.933 * 255, 0.894 * 255, 0.882 * 255, 1.0 * 255);
                SDL_RenderClear(renderer);
                
                SDL_Rect logo_rect = { screenWidth/2 - 200, screenHeight/2 - 45 - 30, 400, 90 };
                SDL_RenderCopy(renderer, logo, NULL, &logo_rect);
                
                //DrawTextEx(font, highscore,Vector2{screenWidth/2 - 37, screenHeight/2 + 10}, 32, 0, BLACK); 
                //DrawTextEx(font, "CLICK ANYWHERE TO BEGIN",Vector2{screenWidth/2 - 134, screenHeight/2 + 50}, 32, 0, ColorFromNormalized((Vector4){.698, .588, .49, 0.4})); 
                
                SDL_RenderPresent(renderer);
                
                if (mouse_pressed) {
                    Mix_PlayChannel(-1, fxSelect, 0);
                    titleScreen = false;
                    mouseDownX = mouse_x;
                    mouseDownY = mouse_y;
                }
            }
            else {
                if (!playedSplash) {
                    Mix_PlayChannel(-1, fxSplash, 0);
                    playedSplash = true;
                }
                
                SDL_SetRenderDrawColor(renderer, 0.933 * 255, 0.894 * 255, 0.882 * 255, 1.0 * 255);
                SDL_RenderClear(renderer);
                
                //DrawTextEx(font, "POLYMARS",Vector2{screenWidth/2 - 54, screenHeight/2 + 3}, 32, 0, ColorFromNormalized((Vector4){.835, .502, .353, 1.0}));
                
                SDL_Rect splashEggSprite_rect = { screenWidth/2 - 16, screenHeight/2 - 16 - 23, 32, 32 };
                SDL_RenderCopy(renderer, splashEggSprite, NULL, &splashEggSprite_rect);
                
                SDL_RenderPresent(renderer);
                
                splashTimer += 1;
            }
        } 
        else {
            if (playCoinFX) {
                Mix_PlayChannel(-1, fxCoin, 0);
                playCoinFX = false;
            }
            
            if (mouse_pressed && player.isOnGround()) {
                mouseDownX = mouse_x;
                mouseDownY = mouse_y;
            }
            
            if (mouse_released && player.isOnGround()) {             
                int velocityX = mouse_x - mouseDownX;
                int velocityY = mouse_y - mouseDownY;
                
                player.setVelocity((double)velocityX*.08, (double)velocityY*.08);    
            }
            
            checkPlayerCollision();
            player.updatePosition();
            
            if (player.getY() > screenHeight) {
                Mix_PlayChannel(-1, fxDeath, 0);
                resetGame();
            }
            for (int i = 0; i < 4; i++) {
                platforms[i].updatePosition();
            }
            
            lavaY = screenHeight - 43 - sin(timer) * 5;
            timer += 0.05;
            
            SDL_SetRenderDrawColor(renderer, 0.933 * 255, 0.894 * 255, 0.882 * 255, 1.0 * 255);
            SDL_RenderClear(renderer);
            
            if (mouse_down && player.isOnGround()) {
                SDL_SetRenderDrawColor(renderer, 0.906*255, 0.847*255, 0.788*255, 1.0*255);
                SDL_RenderDrawLine(
                    renderer,
                    mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2),
                    mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2),
                    mouse_x + (player.getX() - mouseDownX) + (player.getWidth()/2),
                    mouse_y + (player.getY() - mouseDownY) + (player.getHeight()/2)
                );
            }
            
            //DrawRectangle(player.getX(), player.getY(), player.getWidth(), player.getHeight(), WHITE);
            
            for (int i = 0; i < 4; i++) {
                SDL_Rect platformSprite_rect = { platforms[i].getX(), platforms[i].getY(), 100, 32 };
                SDL_RenderCopy(renderer, platformSprite, NULL, &platformSprite_rect);
                
                if (platforms[i].getHasCoin()) {
                    SDL_Rect coinSprite_rect = { platforms[i].getCoinX(), platforms[i].getCoinY(), 24, 24 };
                    SDL_RenderCopy(renderer, coinSprite, NULL, &coinSprite_rect);
                }
            }
            
            SDL_Rect playerSprite_rect = { player.getX(), player.getY(), 32, 32 };
            SDL_RenderCopy(renderer, playerSprite, NULL, &playerSprite_rect);
            
            SDL_Rect lavaSprite_rect = { 0, lavaY, 800, 48 };
            SDL_RenderCopy(renderer, lavaSprite, NULL, &lavaSprite_rect);
            
            SDL_Rect scoreBoxSprite_rect = { 17, 17, 102, 70 };
            SDL_RenderCopy(renderer, scoreBoxSprite, NULL, &scoreBoxSprite_rect);
            
            //DrawTextEx(font, score,Vector2{28, 20},64, 0, BLACK);        
            //DrawTextEx(font, highscore,Vector2{17, 90}, 32, 0, BLACK);        
            
            SDL_RenderPresent(renderer);
        }
       
    }

    SDL_DestroyTexture(playerSprite);
    SDL_DestroyTexture(lavaSprite);
    SDL_DestroyTexture(platformSprite);
    SDL_DestroyTexture(coinSprite);
    SDL_DestroyTexture(scoreBoxSprite);
    SDL_DestroyTexture(logo);
    SDL_DestroyTexture(splashEggSprite);
    
    SDL_FreeSurface(playerSprite_surf);
    SDL_FreeSurface(lavaSprite_surf);
    SDL_FreeSurface(platformSprite_surf);
    SDL_FreeSurface(coinSprite_surf);
    SDL_FreeSurface(scoreBoxSprite_surf);
    SDL_FreeSurface(logo_surf);
    SDL_FreeSurface(splashEggSprite_surf);
    
    Mix_FreeChunk(fxClick);
    Mix_FreeChunk(fxLaunch);
    Mix_FreeChunk(fxDeath);
    Mix_FreeChunk(fxCoin);
    Mix_FreeChunk(fxSplash);
    Mix_FreeChunk(fxSelect);
    
    TTF_CloseFont(font);
    
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();      
   
    return 0;
}
