#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/audioout.h>


#include <vita2d.h>

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include "player.h"
#include "platform.h"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_speech.h"

#define click() (touch.reportNum == 1 & !(oldtouch.reportNum == 1))
#define release() (oldtouch.reportNum == 1 && !(touch.reportNum == 1))
#define hold() (touch.reportNum == 1)

SoLoud::Soloud gSoloud;
SoLoud::Wav fxLaunch;
SoLoud::Wav fxClick;
SoLoud::Wav fxDeath;
SoLoud::Wav fxCoin;
SoLoud::Wav fxSplash;
SoLoud::Wav fxSelect;

const double pi = 3.1415926535897;
const int gravity = 1;

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
     int tempScore = 0;
     std::ifstream save;
     save.open("ux0:data/terri-fried.save", std::ifstream::in );

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
     save.open("ux0:data/terri-fried.save");

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
        platforms[i] = Platform(i);
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
    const int screenWidth = 960;
    const int screenHeight = 544;
    
    int mouseDownX;
    int mouseDownY;
    double lavaY = screenHeight - 32;
    double timer = 0; 
    double splashTimer = 0;
    bool playedSplash = false;
    bool playedSelect = false;
    bool firstTime = true;
    
    gSoloud.init();
    


    vita2d_texture *playerSprite;
    vita2d_texture *lavaSprite;
    vita2d_texture *platformSprite;
    vita2d_texture *coinSprite;
    vita2d_texture *scoreBoxSprite;
    vita2d_texture *logo;
    vita2d_texture *splashEggSprite;
    
    vita2d_font *font;
    vita2d_font *bigFont;
    
    vita2d_init();
    vita2d_set_clear_color(RGBA8(238, 228, 225, 255));
    
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
    
    playerSprite = vita2d_load_PNG_file("app0:resources/egg.png");
    lavaSprite = vita2d_load_PNG_file("app0:resources/lava.png");
    platformSprite = vita2d_load_PNG_file("app0:resources/platform.png");
    coinSprite = vita2d_load_PNG_file("app0:resources/coin.png");
    scoreBoxSprite = vita2d_load_PNG_file("app0:resources/scorebox.png");
    logo = vita2d_load_PNG_file("app0:resources/logo.png");
    splashEggSprite = vita2d_load_PNG_file("app0:resources/splash_egg.png");
    
    vita2d_set_vblank_wait(1);
    
    SceTouchData touch, oldtouch;
	sceTouchPeek(0, &oldtouch, 1);
    
    fxLaunch.load("app0:/resources/launch.ogg"); 
    fxClick.load("app0:/resources/click.ogg"); 
    fxDeath.load("app0:/resources/die.ogg"); 
    fxCoin.load("app0:/resources/coin.ogg"); 
    fxSplash.load("app0:/resources/splash.ogg"); 
    fxSelect.load("app0:/resources/select.ogg"); 
    font = vita2d_load_font_file("app0:resources/font.otf");
    bigFont = vita2d_load_font_file("app0:resources/font.otf");
       
    while (1)   
    {
        sceTouchPeek(0, &touch, 1);
        if (titleScreen)
        {   if (splashTimer > 120)
            {
                if (!playedSelect)
                {
                    gSoloud.play(fxSelect);
                    playedSelect = true;
                }
                vita2d_start_drawing();		
                vita2d_clear_screen();
                vita2d_draw_texture(logo, screenWidth/2 - 200, screenHeight/2 - 45 - 30);
                vita2d_font_draw_text(font, screenWidth/2 - 41, screenHeight/2 + 10 + 32, RGBA8(0, 0, 0, 255), 32, highscore);
                vita2d_font_draw_text(font, screenWidth/2 - 146, screenHeight/2 + 50 + 32, RGBA8(178, 150, 125, 102), 32, "CLICK ANYWHERE TO BEGIN");
                vita2d_end_drawing();
                vita2d_swap_buffers();
                if (click())
                {
                    gSoloud.play(fxSelect);
                    titleScreen = false;
                    mouseDownX = touch.report[0].x;
                    mouseDownY = touch.report[0].y;
                }
            } else {
                if (!playedSplash)
                {
                    gSoloud.play(fxSplash);
                    playedSplash = true;
                }
                vita2d_start_drawing();		
                vita2d_clear_screen();
                vita2d_font_draw_text(font, screenWidth/2 - 59, screenHeight/2 + 3 + 26, RGBA8(213, 128, 90, 255), 32, "POLYMARS");
                vita2d_draw_texture(splashEggSprite, screenWidth/2 - 16, screenHeight/2 - 16 - 23);
                vita2d_end_drawing();
                vita2d_swap_buffers();
                splashTimer += 1;
            }
        } 
        else 
        {
            if (playCoinFX)
            {
                gSoloud.play(fxCoin);
                playCoinFX = false;
            }
            if (click() && player.isOnGround())
            {
                    gSoloud.play(fxClick);
                    mouseDownX = touch.report[0].x;
                    mouseDownY = touch.report[0].y;
            }
            if (release() && player.isOnGround())
            {
                if (firstTime)
                {
                    firstTime = false;
                }
                else 
                {
                    gSoloud.play(fxLaunch);
                    if (player.isOnPlatform())
                    {
                        player.setY(player.getY() - 1);
                    }
                    int velocityX = touch.report[0].x - mouseDownX;
   
                    int velocityY = touch.report[0].y - mouseDownY;

                    player.setVelocity((double)velocityX*.08, (double)velocityY*.08);
                }
            }
            checkPlayerCollision();
            player.updatePosition();
            if (player.getY() > screenHeight)
            {
                gSoloud.play(fxDeath);
                resetGame();
            }
            for (int i = 0; i < 4; i++)
            {
                platforms[i].updatePosition();
            }
        
            lavaY = screenHeight - 43 - sin(timer) * 5;
            timer += 0.05;
            vita2d_start_drawing();		
            vita2d_clear_screen();
            if (hold() && player.isOnGround())
            {   
                vita2d_draw_line(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2), mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2), touch.report[0].x + (player.getX() - mouseDownX) + (player.getWidth()/2), touch.report[0].y + (player.getY() - mouseDownY) + (player.getHeight()/2), RGBA8(231, 216, 201, 255));
                vita2d_draw_line(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2), mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2) + 1, touch.report[0].x + (player.getX() - mouseDownX) + (player.getWidth()/2), touch.report[0].y + (player.getY() - mouseDownY) + (player.getHeight()/2) + 1, RGBA8(231, 216, 201, 255));
                vita2d_draw_line(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2) + 1, mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2), touch.report[0].x + (player.getX() - mouseDownX) + (player.getWidth()/2) + 1, touch.report[0].y + (player.getY() - mouseDownY) + (player.getHeight()/2), RGBA8(231, 216, 201, 255));
                vita2d_draw_line(mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2) + 1, mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2) + 1, touch.report[0].x + (player.getX() - mouseDownX) + (player.getWidth()/2) + 1, touch.report[0].y + (player.getY() - mouseDownY) + (player.getHeight()/2) + 1, RGBA8(231, 216, 201, 255));
            }  
            for (int i = 0; i < 4; i++)
            {
                vita2d_draw_texture(platformSprite, platforms[i].getX(),  platforms[i].getY());
                if (platforms[i].getHasCoin())
                {
                    vita2d_draw_texture(coinSprite, platforms[i].getCoinX(), platforms[i].getCoinY());
                }
            }
            vita2d_draw_texture(playerSprite, player.getX(), player.getY());  
            vita2d_draw_texture(lavaSprite, 0, lavaY); 
            vita2d_draw_texture(scoreBoxSprite, 17, 17); 
            vita2d_font_draw_text(bigFont, 28 - 2, 20 + 64 - 9, RGBA8(0, 0, 0, 255), 64, score);              
            vita2d_font_draw_text(font, 17, 90 + 32, RGBA8(0, 0, 0, 255), 32, highscore);                

            vita2d_end_drawing();
            vita2d_swap_buffers();
        }
        oldtouch = touch;    
       
    }
    
    vita2d_fini();
    vita2d_free_texture(playerSprite);
    vita2d_free_texture(lavaSprite);
    vita2d_free_texture(platformSprite);
    vita2d_free_texture(coinSprite);
    vita2d_free_texture(scoreBoxSprite);
    vita2d_free_texture(logo);
    vita2d_free_texture(splashEggSprite);
    vita2d_free_font(font);
    vita2d_free_font(bigFont);
          
    sceKernelExitProcess(0);
    return 0;
}
