#include <stdio.h>
#include <stdlib.h>

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

#include <grrlib.h>
#include <asndlib.h>
#include "oggplayer.h"

#include "click_ogg.h"
#include "coin_ogg.h"
#include "die_ogg.h"
#include "launch_ogg.h"
#include "select_ogg.h"
#include "splash_ogg.h"

#include "egg_png.h"
#include "lava_png.h"
#include "platform_png.h"
#include "coin_png.h"
#include "scorebox_png.h"
#include "logo_png.h"
#include "splash_egg_png.h"

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include "player.h"
#include "platform.h"
#include "font_ttf.h"

#define WSP_POINTER_CORRECTION_X 200 
#define WSP_POINTER_CORRECTION_Y 250

#define click() (WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A)
#define release() (WPAD_ButtonsUp(WPAD_CHAN_0) & WPAD_BUTTON_A)
#define hold() (WPAD_ButtonsHeld(WPAD_CHAN_0) & WPAD_BUTTON_A)

#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_WHITE   0xFFFFFFFF

const double PI = 3.1415926535897;
const int gravity = 1;
const double sensitivity = 0.5;

Platform platforms[4] = {{0}, {1}, {2}, {3}};
Player player(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2, platforms[0].getY() - player.getHeight(), 26, 32);

int getHighscore();

int scoreInt = 0;
int highscoreInt = getHighscore();
char score[32];
char highscore[32];

bool titleScreen = true;
bool playCoinFX = false;

int getHighscore()
{
     fatInitDefault();
     int tempScore = 0;
     std::ifstream save;
     save.open("/apps/terri-fried/game.sav", std::ifstream::in );

     if( !save.fail() )
     {
        save >> tempScore;
     }
     save.close();
     return tempScore;
}
void saveHighscore()
{
     std::ofstream save;
     save.open("/apps/terri-fried/game.sav");

     save <<  highscoreInt;

     save.close();
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
    player.setX(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2);
    player.setY(platforms[0].getY() - player.getHeight());
}
void checkPlayerCollision()
{
    bool onPlatform = false;
    for (int i = 0; i < 4; i++)
    {
        if (platforms[i].getHasCoin() && player.getX() + player.getWidth() - 3 > platforms[i].getCoinX() && player.getX() + 3 < platforms[i].getCoinX() + 24 && player.getY() + player.getHeight() - 3 > platforms[i].getCoinY() && player.getY() + 3 < platforms[i].getCoinY() + 24)
        {
            addScore(1);
            platforms[i].setHasCoin(false);
            playCoinFX = true;
        }
        if (player.getX() + 1 < platforms[i].getX() + platforms[i].getWidth() && player.getX() + player.getWidth() > platforms[i].getX() && player.getY() + player.getHeight() >= platforms[i].getY() && player.getY() < platforms[i].getY() + platforms[i].getHeight())
        {
            if (player.getY() > platforms[i].getY() + platforms[i].getHeight()/2)
            {
                player.setVelocity(player.getVelocityX(), 5);
            } 
            else if (player.getY() + player.getHeight() <  platforms[i].getY() + platforms[i].getHeight())
            {    
                onPlatform = true;
                player.setY(platforms[i].getY() - player.getHeight());
                player.setY(player.getY() + 1);
            }
        }
        
    }
    player.setOnPlatform(onPlatform);
}

int main(void)
{
    srand (time(NULL));
    resetScore();
    sprintf(highscore, "BEST: %d", highscoreInt);
    const int screenWidth = 640;
    const int screenHeight = 480;
    
    int mouseDownX;
    int mouseDownY;
    double lavaY = screenHeight - 32;
    double timer = 0; 
    double splashTimer = 0;
    bool playedSplash = false;
    bool playedSelect = false;
    bool firstTime = true;
    
    ASND_Init();
    
    GRRLIB_Init();

    GRRLIB_texImg *playerSprite = GRRLIB_LoadTexture(egg_png);
    GRRLIB_texImg *lavaSprite = GRRLIB_LoadTexture(lava_png);
    GRRLIB_texImg *platformSprite = GRRLIB_LoadTexture(platform_png);
    GRRLIB_texImg *coinSprite = GRRLIB_LoadTexture(coin_png);
    GRRLIB_texImg *scoreBoxSprite = GRRLIB_LoadTexture(scorebox_png);
    GRRLIB_texImg *logoSprite = GRRLIB_LoadTexture(logo_png);
    GRRLIB_texImg *splashEggSprite = GRRLIB_LoadTexture(splash_egg_png);
    GRRLIB_ttfFont *font = GRRLIB_LoadTTF(font_ttf, font_ttf_size);

    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
       
    while (1)   
    {
        GRRLIB_FillScreen(0xeee4e1FF); 
        WPAD_ScanPads();
        ir_t ir; 
		WPAD_IR(WPAD_CHAN_0, &ir); 
		
		int touchX = (ir.sx-WSP_POINTER_CORRECTION_X)*sensitivity;
        int touchY = (ir.sy-WSP_POINTER_CORRECTION_Y)*sensitivity;
        
        if(WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME)
            break;
        
        if (titleScreen)
        {   if (splashTimer > 120)
            {
                if (!playedSelect)
                {                    
                    PlayOgg(select_ogg, select_ogg_size, 0, OGG_ONE_TIME);
                    playedSelect = true;
                }
                    
                GRRLIB_DrawImg(screenWidth/2 - 200, screenHeight/2 - 45 - 30, logoSprite, 0, 1, 1, GRRLIB_WHITE);
                   
                GRRLIB_PrintfTTF(screenWidth/2 - 41, screenHeight/2 + 10, font, highscore, 32, 0x000000FF);
                GRRLIB_PrintfTTF(screenWidth/2 - 146, screenHeight/2 + 50, font, "CLICK ANYWHERE TO BEGIN", 32, 0xe7d8c9FF);
        
                if (click())
                {                    
                    PlayOgg(select_ogg, select_ogg_size, 0, OGG_ONE_TIME);
                    titleScreen = false;
                    mouseDownX = touchX;
                    mouseDownY = touchY;
                }
            } else {
                if (!playedSplash)
                {                  
                    PlayOgg(splash_ogg, splash_ogg_size, 0, OGG_ONE_TIME);
                    playedSplash = true;
                }
               
                GRRLIB_PrintfTTF(screenWidth/2 - 59, screenHeight/2 + 3, font, "POLYMARS", 32, 0xd5805aFF);

                GRRLIB_DrawImg(screenWidth/2 - 16, screenHeight/2 - 16 - 23, splashEggSprite, 0, 1, 1, GRRLIB_WHITE);
                
                
               
                splashTimer += 1;
            }
        } 
        else 
        {
            if (playCoinFX)
            {
                PlayOgg(coin_ogg, coin_ogg_size, 0, OGG_ONE_TIME);
                playCoinFX = false;
            }
            if (click() && player.isOnGround())
            {
                    PlayOgg(click_ogg, click_ogg_size, 0, OGG_ONE_TIME);
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
                    PlayOgg(launch_ogg, launch_ogg_size, 0, OGG_ONE_TIME);
                    if (player.isOnPlatform())
                    {
                        player.setY(player.getY() - 1);
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
                PlayOgg(die_ogg, die_ogg_size, 0, OGG_ONE_TIME);
                resetGame();
            }
            for (int i = 0; i < 4; i++)
            {
                platforms[i].updatePosition();
            }
        
            lavaY = screenHeight - 43 - sin(timer) * 5;
            timer += 0.05;

            if (hold() && player.isOnGround())
            {   
                GRRLIB_Line(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2) + 1, mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2) + 1, (touchX + (player.getX() - mouseDownX) + (player.getWidth()/2) + 1), (touchY + (player.getY() - mouseDownY) + (player.getHeight()/2) + 1), 0xe7d8c9FF);
                GRRLIB_Line(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2) + 1, mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2), (touchX + (player.getX() - mouseDownX) + (player.getWidth()/2) + 1), (touchY + (player.getY() - mouseDownY) + (player.getHeight()/2)), 0xe7d8c9FF);
                GRRLIB_Line(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2), mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2) + 1, (touchX + (player.getX() - mouseDownX) + (player.getWidth()/2)), (touchY + (player.getY() - mouseDownY) + (player.getHeight()/2) + 1), 0xe7d8c9FF);
                GRRLIB_Line(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2), mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2), (touchX + (player.getX() - mouseDownX) + (player.getWidth()/2)), (touchY + (player.getY() - mouseDownY) + (player.getHeight()/2)), 0xe7d8c9FF);

            }    
            for (int i = 0; i < 4; i++)
            {
                GRRLIB_DrawImg(platforms[i].getX(), platforms[i].getY(), platformSprite, 0, 1, 1, GRRLIB_WHITE);                 
                if (platforms[i].getHasCoin())
                {     
                     GRRLIB_DrawImg(platforms[i].getCoinX(), platforms[i].getCoinY(), coinSprite, 0, 1, 1, GRRLIB_WHITE); 
                }
            }
                   
            GRRLIB_DrawImg(player.getX(), player.getY(), playerSprite, 0, 1, 1, GRRLIB_WHITE); 
            GRRLIB_DrawImg(0, lavaY, lavaSprite, 0, 1, 1, GRRLIB_WHITE); 
            GRRLIB_DrawImg(17, 17, scoreBoxSprite, 0, 1, 1, GRRLIB_WHITE); 
            
            GRRLIB_PrintfTTF(28 - 2, 20 - 9, font, score, 64, 0x000000FF);            
            GRRLIB_PrintfTTF(17, 90, font, highscore, 32, 0x000000FF);            

   
        } 
        GRRLIB_Render();        
    }
    GRRLIB_FreeTTF(font);
    GRRLIB_FreeTexture(playerSprite);
    GRRLIB_FreeTexture(platformSprite);
    GRRLIB_FreeTexture(coinSprite);
    GRRLIB_FreeTexture(lavaSprite);
    GRRLIB_FreeTexture(scoreBoxSprite);
    GRRLIB_FreeTexture(logoSprite);
    GRRLIB_FreeTexture(splashEggSprite);
    GRRLIB_Exit();
    StopOgg();
    exit(0);
}
