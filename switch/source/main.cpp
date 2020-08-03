#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <switch.h>

#include "player.h"
#include "platform.h"

// some switch buttons
#define JOY_A     0
#define JOY_B     1
#define JOY_X     2
#define JOY_Y     3
#define JOY_PLUS  10
#define JOY_MINUS 11
#define JOY_LEFT  12
#define JOY_UP    13
#define JOY_RIGHT 14
#define JOY_DOWN  15

const double pi = 3.1415926535897;
const int gravity = 1;

Platform platforms[4] = {{0}, {1}, {2}, {3}};
Player player(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2, platforms[0].getY() - player.getHeight(), 26, 32);

int LoadHighScore() {
    FILE *scorefile = fopen("sdmc:/config/terri-fried-score.bin", "rb");
    
    if(!scorefile)
        return 0;
    
    int ret;
    fread(&ret, sizeof(int), 1, scorefile);
    fclose(scorefile);
    
    return ret;
}

void SaveHighScore(int val) {
    FILE *scorefile = fopen("sdmc:/config/terri-fried-score.bin", "wb");
    
    fwrite(&val, sizeof(int), 1, scorefile);
    fclose(scorefile);
}

int scoreInt = 0;
int highscoreInt = LoadHighScore();
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
    SaveHighScore(highscoreInt);
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

void Draw_Font(SDL_Renderer *renderer, const char *str, int x, int y, int width, int height, int size, SDL_Color color) {
    TTF_Font* font = TTF_OpenFont("resources/font.otf", size);
    
    SDL_Surface* message_surf = TTF_RenderText_Blended(font, str, color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, message_surf);
    SDL_Rect Message_rect = {x, y, width, height};
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    
    SDL_DestroyTexture(Message);
    SDL_FreeSurface(message_surf);
    TTF_CloseFont(font);
}

void Draw_Trajectory(SDL_Renderer *renderer, int playerX, int playerY, double velocityX, double velocityY) {
    double XvelStep = velocityX;
    double YvelStep = velocityY;

    double XStep = playerX;
    double YStep = playerY;
    
    double XStepPrev, YStepPrev;

    for(int i = 0; i < 32; i++) {
        if(i != 0) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawLine(renderer, XStep, YStep, XStepPrev, YStepPrev);
        }
        
        XStepPrev = XStep;
        YStepPrev = YStep;
        
        XStep += XvelStep;
        YStep += YvelStep;
        
        YvelStep += gravity;
        
        // FIXME: Not accurate; will cause trajectory to be off slightly
        if (XStep < 16) {
            XvelStep *= -1;
        }
        if (XStep + 16 > 800) { // TODO: Cleanup
            XvelStep *= -1;
        }
    }
}

int main(int argc, char **argv) {
    romfsInit();
    chdir("romfs:/");
    
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
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    
    SDL_Window *window = SDL_CreateWindow(
        "Terri-Fried",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    
    Mix_AllocateChannels(5);
    Mix_OpenAudio(48000, AUDIO_S16, 2, 4096);
    
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_JoystickOpen(0);
    
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
    
    // Gives platform texture a brown color
    SDL_SetTextureColorMod(platformSprite, 0.698*255, 0.588*255, 0.49*255);
    
    Mix_Chunk* fxLaunch = Mix_LoadWAV("resources/launch.wav"); 
    Mix_Chunk* fxClick = Mix_LoadWAV("resources/click.wav"); 
    Mix_Chunk* fxDeath = Mix_LoadWAV("resources/die.wav"); 
    Mix_Chunk* fxCoin = Mix_LoadWAV("resources/coin.wav"); 
    Mix_Chunk* fxSplash = Mix_LoadWAV("resources/splash.wav");
    Mix_Chunk* fxSelect = Mix_LoadWAV("resources/select.wav");
    
    bool quit = false;
    
    bool mouse_down = false;
    
    int mouse_x = 0, mouse_y = 0;
    
    int joy_offsetX = 0, joy_offsetY = 0;
    
    while (!quit) {
        SDL_Event e;
        bool mouse_released = false;
        bool mouse_pressed = false;
        
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT: {
                    quit = true;
                } break;
                case SDL_FINGERDOWN: {
                    mouse_x = e.tfinger.x * screenWidth;
                    mouse_y = e.tfinger.y * screenHeight;
                    
                    mouseDownX = mouse_x;
                    mouseDownY = mouse_y;
                    
                    joy_offsetX = 0;
                    joy_offsetY = 0;
                    
                    mouse_down = true;
                    mouse_pressed = true;
                } break;
                case SDL_FINGERMOTION: {
                    mouse_x = e.tfinger.x * screenWidth;
                    mouse_y = e.tfinger.y * screenHeight;
                } break;
                case SDL_FINGERUP: {
                    mouse_down = false;
                    mouse_released = true;
                } break;
                case SDL_JOYBUTTONDOWN: {
                    if(e.jbutton.button == JOY_PLUS)
                        quit = true;
                    else if(e.jbutton.button == JOY_A) {
                        mouse_x = e.tfinger.x * screenWidth;
                        mouse_y = e.tfinger.y * screenHeight;
                        
                        mouseDownX = mouse_x;
                        mouseDownY = mouse_y;
                        
                        joy_offsetX = 0;
                        joy_offsetY = 0;
                        
                        mouse_down = true;
                        mouse_pressed = true;
                    }
                } break;
                case SDL_JOYBUTTONUP: {
                    if(e.jbutton.button == JOY_A) {
                        mouse_down = false;
                        mouse_released = true;
                    }
                } break;
                case SDL_JOYAXISMOTION: {
                    if(e.jaxis.which == 0) {
                        if(e.jaxis.axis == 0)
                            joy_offsetX = e.jaxis.value;
                        else if(e.jaxis.axis == 1)
                            joy_offsetY = e.jaxis.value;
                    }
                } break;
            }
        }
        
        mouse_x += joy_offsetX/2000;
        mouse_y += joy_offsetY/2000;
        
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
                
                Draw_Font(renderer, highscore, screenWidth/2 - 37, screenHeight/2 + 10, 74, 32, 32, {0, 0, 0});
                Draw_Font(renderer, "CLICK ANYWHERE TO BEGIN", screenWidth/2 - 134, screenHeight/2 + 50, 268, 32, 32, {178, 150, 125});
                
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
                
                Draw_Font(renderer, "POLYMARS", screenWidth/2 - 54, screenHeight/2 + 3, 108, 32, 32, {213, 128, 90});
                
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
                Mix_PlayChannel(-1, fxClick, 0);
                mouseDownX = mouse_x;
                mouseDownY = mouse_y;
            }
            
            int velocityX = mouse_x - mouseDownX;
            int velocityY = mouse_y - mouseDownY;
            
            if (mouse_released && player.isOnGround()) {
                if (firstTime) {
                    firstTime = false;
                }
                else {
                    Mix_PlayChannel(-1, fxLaunch, 0);
                    
                    if(player.isOnPlatform())
                        player.setY(player.getY() - 1);
                    
                    player.setVelocity((double)velocityX*.08, (double)velocityY*.08);
                }
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
                Draw_Trajectory(renderer, player.getX() + 16, player.getY() + 16, (double)velocityX*.08, (double)velocityY*.08);
            }
            
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
            
            Draw_Font(renderer, score, 28, 20, 75, 64, 64, {0, 0, 0});
            Draw_Font(renderer, highscore, 17, 90, 74, 32, 32, {0, 0, 0});
            
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
    
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();      
    romfsExit();
    return 0;
}
