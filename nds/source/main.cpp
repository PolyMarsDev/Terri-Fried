#include <nds.h>
#include <nf_lib.h>
#include <fat.h>

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include "player.h"
#include "platform.h"

#define click() (keysDown() & KEY_TOUCH)
#define release() (keysUp() & KEY_TOUCH)
#define hold() (keysHeld() & KEY_TOUCH)

const double pi = 3.1415926535897;
const int gravity = 1;

Platform platforms[4] = {{0}, {1}, {2}, {3}};
Player player(platforms[0].getX() + platforms[0].getWidth()/2 - 12/2, platforms[0].getY() - player.getHeight(), 12, 16);

bool canSave = fatInitDefault();
int getHighscore();

int scoreInt = 0;
int highscoreInt = getHighscore();
char score[32];
char highscore[32];

bool titleScreen = true;
bool playCoinFX = false;

int getHighscore()
{
     int tempScore = 0;
     if (canSave)
     {
        std::ifstream save;
        if (access("sd:/", F_OK) == 0)
        {
            save.open("sd:/terri-fried.sav", std::ifstream::in );
        } 
        else if (access("fat:/", F_OK) == 0) 
        {
            save.open("fat:/terri-fried.sav", std::ifstream::in );
        }

        if( !save.fail() )
        {
            save >> tempScore;
        }
        save.close();
     }
     return tempScore;
}
void saveHighscore()
{
    if (canSave) 
    {
        std::ofstream save;
        if (access("sd:/", F_OK) == 0)
        {
            save.open("sd:/terri-fried.sav");
        } 
        else if (access("fat:/", F_OK) == 0)
        {
            save.open("fat:/terri-fried.sav");
        }

        save <<  highscoreInt;

        save.close();
    }
}
void addScore(int amount)
{
    scoreInt += amount;
    if (scoreInt < 10)
    {
        sprintf(score, "00%d", scoreInt);
    } else if (scoreInt < 100)
    {
        sprintf(score, "0%d", scoreInt);
    } else {
        sprintf(score, "%d", scoreInt);
    }
    if (scoreInt > highscoreInt)
    {
        highscoreInt = scoreInt;
        sprintf(highscore, "BEST: %d", highscoreInt);
    }
}
void resetScore()
{
    scoreInt = 0;
    sprintf(score, "00%d", scoreInt);
    saveHighscore();
}
void resetGame()
{
    resetScore();
    for (int i = 0; i < 4; i++)
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
    for (int i = 0; i < 4; i++)
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


void drawLine(int startPosX, int startPosY, int endPosX, int endPosY)
{
    for (int i = 0; i < 101; i++)
    {
        if ((endPosY - startPosY)*(i*0.01) + player.getY() + (player.getHeight()/2) - 1 + 2 > 0)
        {
            NF_CreateSprite(0, i + 21, 3, 3, (endPosX - startPosX)*(i*0.01) + (player.getX() + (player.getWidth()/2)) - 1, (endPosY - startPosY)*(i*0.01) + player.getY() + (player.getHeight()/2) - 1);
        }
        else
        {
            NF_CreateSprite(0, i + 21, 3, 3, 256, 192);
        }
    }
}


void hideLine()
{
    for (int i = 0; i < 101; i++)
    {
        NF_CreateSprite(0, i + 21, 3, 3, 256, 192);
    }
}

void drawHighscore(int x, int y)
{
    NF_CreateSprite(0, 4, 8, 8, x, y); //"best:" text
    if (highscoreInt < 10)
    {
        NF_CreateSprite(0, 5, 9, 9, x + 32, y);
        NF_SpriteFrame(0, 5, highscoreInt%10);
    } else if (highscoreInt < 100)
    {
        NF_CreateSprite(0, 5, 9, 9, x + 32, y);
        NF_SpriteFrame(0, 5, highscoreInt/10%10);
        NF_CreateSprite(0, 6, 9, 9, x + 39, y);
        NF_SpriteFrame(0, 6, highscoreInt%10);
    } else {
        NF_CreateSprite(0, 5, 9, 9, x + 32, y);
        NF_SpriteFrame(0, 5, highscoreInt/100%10);
        NF_CreateSprite(0, 6, 9, 9, x + 39, y);
        NF_SpriteFrame(0, 6, highscoreInt/10%10);
        NF_CreateSprite(0, 7, 9, 9, x + 46, y);
        NF_SpriteFrame(0, 7, highscoreInt%10);
    }        
}
void drawLogo(int x, int y)
{
    NF_CreateSprite(0, 8, 10, 10, x, y);
    NF_CreateSprite(0, 9, 10, 10, x + 64, y);
    NF_CreateSprite(0, 10, 10, 10, x + 128, y);
    NF_CreateSprite(0, 11, 10, 10, x, y + 64);
    NF_CreateSprite(0, 12, 10, 10, x + 64, y + 64);
    NF_CreateSprite(0, 13, 10, 10, x + 128, y + 64);
    
    NF_SpriteFrame(0, 9, 1);
    NF_SpriteFrame(0, 10, 2);
    NF_SpriteFrame(0, 11, 3);
    NF_SpriteFrame(0, 12, 4);
    NF_SpriteFrame(0, 13, 5);
}
void hideLogo()
{
    NF_DeleteSprite(0, 8);
    NF_DeleteSprite(0, 9);
    NF_DeleteSprite(0, 10);
    NF_DeleteSprite(0, 11);
    NF_DeleteSprite(0, 12);
    NF_DeleteSprite(0, 13);
}
int main(void)
{
    srand (time(NULL));
    resetScore();
    sprintf(highscore, "BEST: %d", highscoreInt);
    const int screenWidth = 256;
    const int screenHeight = 192;
    
    int mouseDownX;
    int mouseDownY;
    double lavaY = screenHeight - 16;
    double timer = 0; 
    double splashTimer = 0;
    bool playedSplash = false;
    bool playedSelect = false;
    bool firstTime = true;
    
    consoleDemoInit();	
	consoleClear();		
	soundEnable();
    NF_Set2D(0,0);
	NF_SetRootFolder("NITROFS");
    
    NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);
	
	NF_LoadTiledBg("backgrounds/bg", "bg", 256, 256);
	NF_CreateTiledBg(0, 3, "bg");
    
    NF_LoadTiledBg("backgrounds/lower_bg", "lower_bg", 256, 256);
	NF_CreateTiledBg(1, 3, "lower_bg");
    
    NF_InitSpriteBuffers();
	NF_InitSpriteSys(0);
	
	NF_LoadSpriteGfx("sprites/player", 0, 16, 16); 
	NF_LoadSpritePal("sprites/player", 0);
	NF_VramSpriteGfx(0, 0, 0, false);
	NF_VramSpritePal(0, 0, 0);
    
    NF_LoadSpriteGfx("sprites/splash", 1, 64, 64); 
	NF_LoadSpritePal("sprites/splash", 1);
	NF_VramSpriteGfx(0, 1, 1, false);
	NF_VramSpritePal(0, 1, 1);
    
    NF_LoadSpriteGfx("sprites/platform", 2, 64, 32); 
	NF_LoadSpritePal("sprites/platform", 2);
	NF_VramSpriteGfx(0, 2, 2, false);
	NF_VramSpritePal(0, 2, 2);
    
    NF_LoadSpriteGfx("sprites/node", 3, 8, 8); 
	NF_LoadSpritePal("sprites/node", 3);
	NF_VramSpriteGfx(0, 3, 3, true);
	NF_VramSpritePal(0, 3, 3);
    
    NF_LoadSpriteGfx("sprites/lava", 4, 64, 32); 
	NF_LoadSpritePal("sprites/lava", 4);
	NF_VramSpriteGfx(0, 4, 4, false);
	NF_VramSpritePal(0, 4, 4);
    
    NF_LoadSpriteGfx("sprites/coin", 5, 16, 16); 
	NF_LoadSpritePal("sprites/coin", 5);
	NF_VramSpriteGfx(0, 5, 5, false);
	NF_VramSpritePal(0, 5, 5);
    
    NF_LoadSpriteGfx("sprites/scorebox", 6, 64, 64); 
	NF_LoadSpritePal("sprites/scorebox", 6);
	NF_VramSpriteGfx(0, 6, 6, false);
	NF_VramSpritePal(0, 6, 6);
    
    NF_LoadSpriteGfx("sprites/score", 7, 16, 32); 
	NF_LoadSpritePal("sprites/score", 7);
	NF_VramSpriteGfx(0, 7, 7, false);
	NF_VramSpritePal(0, 7, 7);
    
    NF_LoadSpriteGfx("sprites/besttext", 8, 32, 16); 
	NF_LoadSpritePal("sprites/besttext", 8);
	NF_VramSpriteGfx(0, 8, 8, false);
	NF_VramSpritePal(0, 8, 8);
    
    NF_LoadSpriteGfx("sprites/highscore", 9, 8, 16); 
	NF_LoadSpritePal("sprites/highscore", 9);
	NF_VramSpriteGfx(0, 9, 9, false);
	NF_VramSpritePal(0, 9, 9);
    
    NF_LoadSpriteGfx("sprites/logo", 10, 64, 64); 
	NF_LoadSpritePal("sprites/logo", 10);
	NF_VramSpriteGfx(0, 10, 10, false);
	NF_VramSpritePal(0, 10, 10);
    
    NF_LoadSpriteGfx("sprites/lava_bg", 11, 64, 32); 
	NF_LoadSpritePal("sprites/lava_bg", 11);
	NF_VramSpriteGfx(0, 11, 11, false);
	NF_VramSpritePal(0, 11, 11);
    
    touchPosition touch;
    
    NF_InitRawSoundBuffers();
	
	NF_LoadRawSound("sounds/click", 0, 11025, 0);
	NF_LoadRawSound("sounds/coin", 1, 11025, 0);
	NF_LoadRawSound("sounds/die", 2, 11025, 0);
	NF_LoadRawSound("sounds/launch", 3, 11025, 0);
	NF_LoadRawSound("sounds/select", 4, 11025, 0);
    NF_LoadRawSound("sounds/splash", 5, 11025, 0);
       
    while (1)   
    {
        scanKeys();
		touchRead(&touch);
        
        if (keysDown() & KEY_START)
        {
            break;
        }
        int touchX;
        int touchY;
        
        if(hold())
        {
            touchX = touch.px;
            touchY = touch.py;
        }
        
        if (titleScreen)
        {   if (splashTimer > 120)
            {
                if (!playedSelect)
                {                    
                    NF_PlayRawSound(4, 127, 64, false, 0);
                    playedSelect = true;
                    NF_DeleteSprite(0, 0);
                }
                drawLogo(128 - 96, 96 - 64);
                //drawHighscore(128 - 20, 96 + 6);
                if (click())
                {
                    hideLogo();
                    NF_PlayRawSound(4, 127, 64, false, 0);
                    titleScreen = false;
                    mouseDownX = touchX;
                    mouseDownY = touchY;
                }
            } else {
                if (!playedSplash)
                {
                    NF_PlayRawSound(5, 127, 64, false, 0);                   
                    playedSplash = true;
                }
               NF_CreateSprite(0, 0, 1, 1, 256/2 - 32, 192/2 - 32);
               splashTimer += 1;
            }
        } 
        else 
        {
            if (playCoinFX)
            {
                NF_PlayRawSound(1, 127, 64, false, 0);
                playCoinFX = false;
            }
            if (click() && player.isOnGround())
            {
                    NF_PlayRawSound(0, 127, 64, false, 0);
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
                    NF_PlayRawSound(3, 127, 64, false, 0);
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
                NF_PlayRawSound(2, 127, 64, false, 0);
                resetGame();
            }
            for (int i = 0; i < 4; i++)
            {
                platforms[i].updatePosition();
            }
        
            lavaY = screenHeight - 29 - sin(timer) * 2.5;
            timer += 0.05;
            if (hold() && player.isOnGround())
            {   
                drawLine(mouseDownX, mouseDownY, touchX, touchY);
            }  
            else
            {
                hideLine();
            }
            for (int i = 0; i < 4; i++)
            {
                if (platforms[i].getY() + platforms[i].getHeight() > 0)
                {
                    NF_CreateSprite(0, i + 13, 2, 2, platforms[i].getX(), platforms[i].getY());
                    if (platforms[i].getHasCoin())
                    {
                        NF_CreateSprite(0, 17 + i, 5, 5, platforms[i].getCoinX(), platforms[i].getCoinY());
                    }
                    else {
                        NF_CreateSprite(0, 17 + i, 5, 5, 256, 192);
                    }
                } 
                else {
                    NF_CreateSprite(0, i + 13, 2, 2, 256, 192); //draw platform offscreen
                    NF_CreateSprite(0, 17 + i, 5, 5, 256, 192);
                }
                
            }
            if (player.getY() + player.getHeight() > 0)
            {
                NF_CreateSprite(0, 12, 0, 0, player.getX(), player.getY());
            }
            else
            {
                NF_CreateSprite(0, 12, 0, 0, 256, 192);
            }
            
     
            NF_CreateSprite(0, 8, 4, 4, 0, lavaY);
            NF_CreateSprite(0, 9, 4, 4, 64, lavaY);
            NF_CreateSprite(0, 10, 4, 4, 128, lavaY);
            NF_CreateSprite(0, 11, 4, 4, 192, lavaY);
            
            NF_SpriteFrame(0, 8, 0);
            NF_SpriteFrame(0, 9, 1);
            NF_SpriteFrame(0, 10, 2);
            NF_SpriteFrame(0, 11, 3); 
            
            NF_CreateSprite(0, 122, 11, 11, 0, lavaY + 1);
            NF_CreateSprite(0, 123, 11, 11, 64, lavaY + 1);
            NF_CreateSprite(0, 124, 11, 11, 128, lavaY + 1);
            NF_CreateSprite(0, 125, 11, 11, 192, lavaY + 1);
            
            NF_SpriteFrame(0, 122, 0);
            NF_SpriteFrame(0, 123, 1);
            NF_SpriteFrame(0, 124, 2);
            NF_SpriteFrame(0, 125, 3);
            
            
            //NF_CreateSprite(0, 0, 7, 7, 13, 14); //score text
            //NF_CreateSprite(0, 1, 7, 7, 27, 14);
            //NF_CreateSprite(0, 2, 7, 7, 41, 14);
            if (scoreInt != 0)
            {
                if (scoreInt > 99) 
                {
                   //NF_SpriteFrame(0, 0, scoreInt/100%10);
                }
                if (scoreInt > 9)
                {
                    //NF_SpriteFrame(0, 1, scoreInt/10%10);
                }
                //NF_SpriteFrame(0, 2, scoreInt%10);
            }
            //NF_CreateSprite(0, 3, 6, 6, 8, 8); //score box
            //drawHighscore(8, 48);
        }
        NF_SpriteOamSet(0);
		swiWaitForVBlank();
		oamUpdate(&oamMain);
    }
    return 0;
}
