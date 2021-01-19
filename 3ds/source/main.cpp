#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <SDL/SDL.H>
#include <SDL/SDL_MIXER.H>

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include "player.h"
#include "platform.h"
#include <iostream>

u32 kDown;
u32 kHeld;
u32 kUp;
#define click() (kDown & KEY_TOUCH)
#define release() (kUp & KEY_TOUCH)
#define hold() (kHeld & KEY_TOUCH)

#define pnum 5
#define sboxpos 7

const double pi = 3.1415926535897;
const float gravity = 0.5f;

Platform platforms[pnum] = {{0}, {1}, {2}, {3}, {4}};
Player player(platforms[0].getX() + platforms[0].getWidth()/2 - 12/2, platforms[0].getY() - player.getHeight(), 12, 16);


int scoreInt = 0;

char score[32];
char highscore[32];

bool titleScreen = true;
bool playCoinFX = false;


typedef struct numbers
{
public:
    C2D_Sprite sbox1[10];
    C2D_Sprite sbox2[10];
    C2D_Sprite sbox3[10];
} numbers;

int LoadHighScore() 
{
    FILE *scorefile = fopen("/3ds/highscore.bin", "rb");

    if(!scorefile)
        return 0;
    
    int ret;
    fread(&ret, sizeof(int), 1, scorefile);
    fclose(scorefile);
    
    return ret;
}

int highscoreInt = LoadHighScore();

void SaveHighScore(int val) 
{
    FILE *scorefile = fopen("/3ds/highscore.bin", "wb");
    
    fwrite(&val, sizeof(int), 1, scorefile);
    fclose(scorefile);
}

void addScore(int amount)
{
    scoreInt += amount;
    if (scoreInt > highscoreInt)
    {
        highscoreInt = scoreInt;
        SaveHighScore(highscoreInt);
    }
}

void drawscore(int score, numbers &s)
{
    if (score < 10)
    {
        C2D_DrawSprite(&s.sbox1[0]);
        C2D_DrawSprite(&s.sbox2[0]);
        C2D_DrawSprite(&s.sbox3[score]);
    }
       if (score < 100)
    {
        C2D_DrawSprite(&s.sbox1[0]);
        C2D_DrawSprite(&s.sbox2[(int)score/10]);
        C2D_DrawSprite(&s.sbox3[score - (((int)score/10) * 10)]);
    }
           if (score < 1000)
    {
        C2D_DrawSprite(&s.sbox1[(int)score/100]);
        C2D_DrawSprite(&s.sbox2[(int)(score- (((int)score/100) * 100))/10]);
        C2D_DrawSprite(&s.sbox3[score - (((int)score/10) * 10)]);
    }
}

void drawhighscore(int score, numbers &s)
{
    switch(score)
    {
        case 0 ... 9:
        C2D_DrawSprite(&s.sbox1[score]);
        break;
        case 10 ... 99:
        C2D_DrawSprite(&s.sbox1[(int)score/10]);
        C2D_DrawSprite(&s.sbox2[score - (((int)score/10) * 10)]);
        break;
        case 100 ... 999:
        C2D_DrawSprite(&s.sbox1[(int)score/100]);
        C2D_DrawSprite(&s.sbox2[(int)(score- (((int)score/100) * 100))/10]);
        C2D_DrawSprite(&s.sbox3[score - (((int)score/10) * 10)]);
        break;
    }

}

void resetScore()
{
    scoreInt = 0;
}

void resetGame()
{
    resetScore();
    for (int i = 0; i < pnum; i++)
    {
        platforms[i].resetPosition(i);
    }
    player.setVelocity(0, 0);
    player.setX(platforms[0].getX() + platforms[0].getWidth()/2 - 12/2);
    player.setY(platforms[0].getY() - player.getHeight());
}

void checkPlayerCollision()
{
    bool onPlatform = false;
    for (int i = 0; i < pnum; i++)
    {
        if (platforms[i].getHasCoin() && player.getX() + player.getWidth() - 1 > platforms[i].getCoinX() && player.getX() + 1 < platforms[i].getCoinX() + 12 && player.getY() + player.getHeight() - 1 > platforms[i].getCoinY() && player.getY() + 1 < platforms[i].getCoinY() + 12)
        {
            addScore(1);
            platforms[i].setHasCoin(false);
            playCoinFX = true;
        }
        if (player.getX() + 1 < platforms[i].getX() + platforms[i].getWidth() && player.getX() + player.getWidth() > platforms[i].getX() && player.getY() + player.getHeight() >= platforms[i].getY() && player.getY() < platforms[i].getY() + platforms[i].getHeight())
        {
            if (player.getY() > platforms[i].getY() + platforms[i].getHeight()/2)
            {
                player.setVelocity(player.getVelocityX(), 2.5);
            } 
            else if (player.getY() + player.getHeight() <  platforms[i].getY() + platforms[i].getHeight())
            {    
                onPlatform = true;
                player.setY(platforms[i].getY() - player.getHeight());
                player.setY(player.getY() + 0.5);
            }
        }
        
    }
    player.setOnPlatform(onPlatform);
}


bool line;
int touchX;
int touchY;

int main(void)
{
    srand (time(NULL));
    const int screenWidth = 400;
    const int screenHeight = 240;
    
    int mouseDownX;
    int mouseDownY;
    double lavaY = screenHeight - 16;
    double timer = 0; 
    double splashTimer = 0;
    bool playedSplash = false;
    bool playedSelect = false;
    bool firstTime = true;
    
    romfsInit();
    gfxInitDefault();
    gfxSet3D(true);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS * 2);
    C2D_Prepare();

    SDL_Init(SDL_INIT_AUDIO);
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0);
    std::cout << Mix_GetError();

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	C2D_SpriteSheet spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
    C2D_SpriteSheet sboxspritesheet = C2D_SpriteSheetLoad("romfs:/gfx/numbers.t3x");

    C2D_Sprite egg;
    C2D_Sprite coin[pnum];
    C2D_Sprite platform[pnum];
    C2D_Sprite logo;
    C2D_Sprite title;
    C2D_Sprite Lava;
    C2D_Sprite sboxn;
    C2D_Sprite best;
    C2D_Sprite tap;
    C2D_Sprite quit;

    numbers scoreboard;
    numbers highscore;

    Mix_Chunk *click = Mix_LoadWAV("romfs:sounds/click.wav");
    Mix_Chunk *boot = Mix_LoadWAV("romfs:sounds/splash.wav");
    Mix_Chunk *launch = Mix_LoadWAV("romfs:sounds/launch.wav");
    Mix_Chunk *coinget = Mix_LoadWAV("romfs:sounds/coin.wav");
    Mix_Chunk *splash = Mix_LoadWAV("romfs:sounds/select.wav");
    Mix_Chunk *die = Mix_LoadWAV("romfs:sounds/die.wav");


    for(int i = 0; i <10; i++)
    {
        C2D_SpriteFromSheet(&scoreboard.sbox1[i], sboxspritesheet, i);
        C2D_SpriteSetCenter(&scoreboard.sbox1[i], 0.5, 0.5);
        C2D_SpriteSetScale(&scoreboard.sbox1[i], 1.2,1.5);
        C2D_SpriteSetPos(&scoreboard.sbox1[i], (10 + sboxpos), (17+ sboxpos)); 

        C2D_SpriteFromSheet(&scoreboard.sbox2[i], sboxspritesheet, i);
        C2D_SpriteSetCenter(&scoreboard.sbox2[i], 0.5, 0.5);
        C2D_SpriteSetScale(&scoreboard.sbox2[i], 1.2,1.5);
        C2D_SpriteSetPos(&scoreboard.sbox2[i], (25 + sboxpos), (17 + sboxpos)); 

        C2D_SpriteFromSheet(&scoreboard.sbox3[i], sboxspritesheet, i);
        C2D_SpriteSetCenter(&scoreboard.sbox3[i], 0.5, 0.5);
        C2D_SpriteSetScale(&scoreboard.sbox3[i], 1.2,1.5);
        C2D_SpriteSetPos(&scoreboard.sbox3[i], (40 + sboxpos), (17 + sboxpos)); 

        C2D_SpriteFromSheet(&highscore.sbox1[i], sboxspritesheet, i);
        C2D_SpriteSetCenter(&highscore.sbox1[i], 0.5, 0.5);
        C2D_SpriteSetScale(&highscore.sbox1[i], 1,1.3);

        C2D_SpriteFromSheet(&highscore.sbox2[i], sboxspritesheet, i);
        C2D_SpriteSetCenter(&highscore.sbox2[i], 0.5, 0.5);
        C2D_SpriteSetScale(&highscore.sbox2[i], 1,1.3);

        C2D_SpriteFromSheet(&highscore.sbox3[i], sboxspritesheet, i);
        C2D_SpriteSetCenter(&highscore.sbox3[i], 0.5, 0.5);
        C2D_SpriteSetScale(&highscore.sbox3[i], 1,1.3);
    }

    for (int i = 0; i < pnum;i++)
    {
        C2D_SpriteFromSheet(&platform[i], spriteSheet, 4);
        C2D_SpriteSetCenter(&platform[i], 0, 0.5);
        C2D_SpriteSetScale(&platform[i], 1,1);

        C2D_SpriteFromSheet(&coin[i], spriteSheet, 0);    
        C2D_SpriteSetCenter(&coin[i], 0, 0.79);
        C2D_SpriteSetScale(&coin[i], 1,1);
    }
    
    C2D_SpriteFromSheet(&logo, spriteSheet, 6);
    C2D_SpriteSetCenter(&logo, 0.5, 0.5);
    C2D_SpriteSetScale(&logo, 1,1);
    C2D_SpriteSetPos(&logo, 200, 120 - 30);

    C2D_SpriteFromSheet(&Lava, spriteSheet, 2);
    C2D_SpriteSetCenter(&Lava, 0.5, 0.5);
    C2D_SpriteSetScale(&Lava, 1,1);
    C2D_SpriteSetPos(&Lava, 120, lavaY);

    C2D_SpriteFromSheet(&sboxn, spriteSheet, 5);
    C2D_SpriteSetCenter(&sboxn, 0, 0);
    C2D_SpriteSetScale(&sboxn, 0.5,0.5);
    C2D_SpriteSetPos(&sboxn, sboxpos, sboxpos);  

    C2D_SpriteFromSheet(&egg, spriteSheet, 1);
    C2D_SpriteSetCenter(&egg, 0, 0.79);
    C2D_SpriteSetScale(&egg, 1,1);
    C2D_SpriteSetPos(&egg, 400, 240); 

    C2D_SpriteFromSheet(&best, spriteSheet, 7);
    C2D_SpriteSetCenter(&best, 0, 0);
    C2D_SpriteSetScale(&best, 1,1);
    C2D_SpriteSetPos(&best, sboxpos, 52);

    C2D_SpriteFromSheet(&tap, spriteSheet, 8);
    C2D_SpriteSetCenter(&tap, 0, 0);
    C2D_SpriteSetScale(&tap, 1,1);
    C2D_SpriteSetPos(&tap, 50, 150);

    C2D_SpriteFromSheet(&quit, spriteSheet, 9);
    C2D_SpriteSetPos(&quit, 40, 200);  
    C2D_SpriteSetCenter(&quit, 0, 0);
    C2D_SpriteSetScale(&quit, 1,1);
    

    while (aptMainLoop())   
    {
        hidScanInput();
        touchPosition touch;
        hidTouchRead(&touch);
        kDown = hidKeysDown();
        kHeld = hidKeysHeld();
        kUp   = hidKeysUp();

        if (kDown & KEY_START)
        {
            break;
        }
      
        
        if(hold())
        {
            touchX = touch.px;
            touchY = touch.py;
        }
        C2D_SpriteSetPos(&best, sboxpos, 52); 
        if (titleScreen)
        {   
            if (splashTimer > 120)
            {
                if (!playedSelect)
                {    
                      Mix_PlayChannel(-1, splash, 0);             
                    playedSelect = true;
                }
                    for(int i = 0; i <10; i++)
                    {
                    C2D_SpriteSetPos(&highscore.sbox1[i], 195, 130); 
                    C2D_SpriteSetPos(&highscore.sbox2[i], 205, 130); 
                    C2D_SpriteSetPos(&highscore.sbox3[i], 215, 130); 
                    }
                    C2D_SpriteFromSheet(&title, spriteSheet, 3);
                    C2D_SpriteSetScale(&title, 1,1);
                    C2D_SpriteSetPos(&best, 150, 125);
                    C2D_SpriteSetCenter(&title, 0, 0.5);
                    C2D_SpriteSetPos(&title, 90 - 32, 120 - 30);     
                    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
                    C2D_TargetClear(top, C2D_Color32(239, 231, 231, 255));
                    C2D_SceneBegin(top);
                    C2D_DrawSprite(&title);
                    C2D_DrawSprite(&best);
                    C2D_DrawSprite(&tap);
                    drawhighscore(highscoreInt, highscore);
                    C3D_FrameEnd(0);

                if (click())
                {
                    titleScreen = false;
                    mouseDownX = touchX;
                    mouseDownY = touchY;
                }
            } 
            else 
            {
                if (!playedSplash)
                {
                    Mix_PlayChannel(-1, boot, 0);                
                    playedSplash = true;
                }
                    C2D_SpriteFromSheet(&title, spriteSheet, 10);
                    C2D_SpriteSetScale(&title, 1,1);
                    C2D_SpriteSetCenter(&title, 0, 0.5);
                    C2D_SpriteSetPos(&title, 130, 120); 
                    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
                    C2D_TargetClear(top, C2D_Color32(239, 231, 231, 255));
                    C2D_SceneBegin(top);
                    C2D_DrawSprite(&logo);
                    C2D_DrawSprite(&title);
                    C2D_SceneBegin(bottom);
                    C2D_TargetClear(bottom, C2D_Color32f(0, 0, 0, 1.0));
                    C2D_DrawSprite(&quit);
                    C3D_FrameEnd(0);
               splashTimer += 1;
            }
        } 
        else 
        {
            C2D_SpriteSetPos(&best, sboxpos, 52); 

            for(int i = 0; i <10; i++)
            {
            C2D_SpriteSetPos(&highscore.sbox1[i], (45 + sboxpos), 58); 
            C2D_SpriteSetPos(&highscore.sbox2[i], (55 + sboxpos), 58); 
            C2D_SpriteSetPos(&highscore.sbox3[i], (65 + sboxpos), 58); 
            }

            if (playCoinFX)
            {
                Mix_PlayChannel(-1, coinget, 0); 
                playCoinFX = false;
            }

            if (click() && player.isOnGround())
            {
                Mix_PlayChannel(-1, click, 0);
                    mouseDownX = touchX;
                    mouseDownY = touchY;
            }

            if (release() && player.isOnGround())
            {
                if (firstTime)
                {
                    firstTime = false;
                }
                else 
                {
                    Mix_PlayChannel(-1, launch, 0);
                    if (player.isOnPlatform())
                    {
                        player.setY(player.getY() - 0.5);
                    }
                    int velocityX = touchX - mouseDownX;
   
                    int velocityY = touchY - mouseDownY;

                    player.setVelocity((double)velocityX*.08, (double)velocityY*.08);
                }
            }
            checkPlayerCollision();
            player.updatePosition();

            if (player.getY() > screenHeight)
            {
                Mix_PlayChannel(-1, die, 0); 
                resetGame();
            }

            for (int i = 0; i < pnum; i++)
            {
                platforms[i].updatePosition();
            }

            lavaY = screenHeight - 13 - sin(timer) * 2.5;
            timer += 0.05;
            C2D_SpriteSetPos(&Lava, 120, lavaY);

            if (hold() && player.isOnGround())
            {   
                line = true;
            }  
            else
            {
                line = false;
            }
            for (int i = 0; i < pnum; i++)
            {
                if (platforms[i].getY() + platforms[i].getHeight() > 0)
                {
                    C2D_SpriteSetPos(&platform[i], platforms[i].getX(), platforms[i].getY());
                 
                    if (platforms[i].getHasCoin())
                    {
                         C2D_SpriteSetPos(&coin[i], platforms[i].getCoinX(), platforms[i].getCoinY());
                    }
                    else {
                         C2D_SpriteSetPos(&coin[i], 400,240);
                    }
                } 
                else {
                    C2D_SpriteSetPos(&platform[i], 400, 240);
                    C2D_SpriteSetPos(&coin[i], 400,240);
                }
                
            }
            if (player.getY() + player.getHeight() > 0)
            {
                C2D_SpriteSetPos(&egg, player.getX(), player.getY());
            }
            else
            {
                C2D_SpriteSetPos(&egg, 400, 240);
            }
            
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            C2D_TargetClear(top, C2D_Color32(239, 231, 231, 255));
            C2D_SceneBegin(top);
            if (line)
            {
                if ((touchY - mouseDownY) + player.getY() + (player.getHeight()/2) - 1 + 2 > 0)
                {
                    C2D_DrawLine((mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()*1.5)) - 7,
                    (mouseDownY + (player.getY() - mouseDownY) - (player.getHeight()/1.7)-5) + 5,
                    C2D_Color32f(0.906, 0.847, 0.788, 1.0),
                    (touchX - mouseDownX) + (player.getX() + (player.getWidth()*1.5)) -7,
                    (touchY - mouseDownY) + player.getY() - (player.getHeight()/1.7) ,
                    C2D_Color32f(0.906, 0.847, 0.788, 1.0),
                    3,
                    0);
                }
                 
               
            }
            for(int i = 0;i < pnum;i++)
            {
                C2D_DrawSprite(&platform[i]);
                C2D_DrawSprite(&coin[i]);    
            }            
            C2D_DrawSprite(&egg);
            C2D_DrawSprite(&Lava);
            C2D_DrawSprite(&sboxn);
            drawscore(scoreInt, scoreboard);
            drawhighscore(highscoreInt, highscore);
            C2D_DrawSprite(&best);
            C3D_FrameEnd(0);
            
        }
         
    }
    Mix_FreeChunk(click);
    Mix_FreeChunk(boot);
    Mix_FreeChunk(launch);
    Mix_FreeChunk(coinget);
    Mix_FreeChunk(splash);
    Mix_FreeChunk(die);
    C2D_SpriteSheetFree(sboxspritesheet);
    C2D_SpriteSheetFree(spriteSheet); 
    Mix_Quit();
    SDL_Quit();
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    return 0;
}
