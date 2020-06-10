#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "player.h"
#include "platform.h"

const double pi = 3.1415926535897;


const int gravity = 1;



Platform platforms[4] = {{0}, {1}, {2}, {3}};
Player player(platforms[0].getX() + platforms[0].getWidth()/2 - 26/2, platforms[0].getY() - player.getHeight(), 26, 32);

int scoreInt = 0;
int highscoreInt = LoadStorageValue(0);
char score[32];
char highscore[32];

bool titleScreen = true;
bool playCoinFX = false;

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
    SaveStorageValue(0, highscoreInt);
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
                player.setVelocity(player.getVelocity().x, 5);
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
    Image egg = LoadImage("resources/egg.png");
    InitWindow(screenWidth, screenHeight, "Terri-Fried");
    SetWindowIcon(egg);
    InitAudioDevice();
    SetMasterVolume(0.3f);
    
    Texture2D playerSprite = LoadTexture("resources/egg.png");
    Texture2D lavaSprite = LoadTexture("resources/lava.png");
    Texture2D platformSprite = LoadTexture("resources/platform.png");
    Texture2D coinSprite = LoadTexture("resources/coin.png");
    Texture2D scoreBoxSprite = LoadTexture("resources/scorebox.png");
    Texture2D logo = LoadTexture("resources/logo.png");
    Texture2D splashEggSprite = LoadTexture("resources/splash_egg.png");
    
    Sound fxLaunch = LoadSound("resources/launch.wav"); 
    Sound fxClick = LoadSound("resources/click.wav"); 
    Sound fxDeath = LoadSound("resources/die.wav"); 
    Sound fxCoin = LoadSound("resources/coin.wav"); 
    Sound fxSplash = LoadSound("resources/splash.wav");
    Sound fxSelect = LoadSound("resources/select.wav"); 
    Font font = LoadFontEx("resources/font.otf", 64, 0, NULL);
    SetTargetFPS(60);              
    while (!WindowShouldClose())   
    {
        if (titleScreen)
        {   if (splashTimer > 120)
            {
                if (!playedSelect)
                {
                    PlaySound(fxSelect);
                    playedSelect = true;
                }
                BeginDrawing();
                ClearBackground(ColorFromNormalized((Vector4){0.933, 0.894, 0.882, 1.0}));
                DrawTexture(logo, screenWidth/2 - 200, screenHeight/2 - 45 - 30, WHITE);
                DrawTextEx(font, highscore,Vector2{screenWidth/2 - 37, screenHeight/2 + 10}, 32, 0, BLACK); 
                DrawTextEx(font, "CLICK ANYWHERE TO BEGIN",Vector2{screenWidth/2 - 134, screenHeight/2 + 50}, 32, 0, ColorFromNormalized((Vector4){.698, .588, .49, 0.4})); 
                EndDrawing();
                if (IsMouseButtonPressed(0))
                {
                    PlaySound(fxSelect);
                    titleScreen = false;
                    mouseDownX = GetMouseX();
                    mouseDownY = GetMouseY();
                }
            } else {
                if (!playedSplash)
                {
                    PlaySound(fxSplash);
                    playedSplash = true;
                }
                BeginDrawing();
                ClearBackground(ColorFromNormalized((Vector4){0.933, 0.894, 0.882, 1.0}));
                DrawTextEx(font, "POLYMARS",Vector2{screenWidth/2 - 54, screenHeight/2 + 3}, 32, 0, ColorFromNormalized((Vector4){.835, .502, .353, 1.0})); 
                DrawTexture(splashEggSprite, screenWidth/2 - 16, screenHeight/2 - 16 - 23, WHITE);
                EndDrawing();
                splashTimer += 1;
            }
        } 
        else 
        {
            if (playCoinFX)
            {
                PlaySound(fxCoin);
                playCoinFX = false;
            }
            
            
            if (IsMouseButtonPressed(0) && player.isOnGround())
            {
                mouseDownX = GetMouseX();
                mouseDownY = GetMouseY();
            }
            
            
            if (IsMouseButtonReleased(0) && player.isOnGround())
            {             
                int velocityX = GetMouseX() - mouseDownX;
                int velocityY = GetMouseY() - mouseDownY;
                
                player.setVelocity((double)velocityX*.08, (double)velocityY*.08);    
            }
            
            checkPlayerCollision();
            player.updatePosition();
            if (player.getY() > screenHeight)
            {
                PlaySound(fxDeath);
                resetGame();
            }
            for (int i = 0; i < 4; i++)
            {
                platforms[i].updatePosition();
            }
        
            lavaY = screenHeight - 43 - sin(timer) * 5;
            timer += 0.05;
            BeginDrawing();
        
            ClearBackground(ColorFromNormalized((Vector4){0.933, 0.894, 0.882, 1.0}));
            if (IsMouseButtonDown(0) && player.isOnGround())
            {   
                DrawLineEx((Vector2){mouseDownX + (player.getX() - mouseDownX) + (player.getWidth()/2), mouseDownY + (player.getY() - mouseDownY) + (player.getHeight()/2)}, (Vector2){GetMouseX() + (player.getX() - mouseDownX) + (player.getWidth()/2), GetMouseY() + (player.getY() - mouseDownY) + (player.getHeight()/2)}, 3.0f, ColorFromNormalized((Vector4){.906, .847, .788, 1.0}));
            }
            //DrawRectangle(player.getX(), player.getY(), player.getWidth(), player.getHeight(), WHITE);  
            for (int i = 0; i < 4; i++)
            {
                DrawTexture(platformSprite, platforms[i].getX(), platforms[i].getY(), ColorFromNormalized((Vector4){.698, .588, .49, 1.0}));       
                if (platforms[i].getHasCoin())
                {
                    DrawTexture(coinSprite, platforms[i].getCoinX(), platforms[i].getCoinY(), WHITE);
                }
            }
            DrawTexture(playerSprite, player.getX(), player.getY(), WHITE);  
            DrawTexture(lavaSprite, 0, lavaY, WHITE); 
            DrawTexture(scoreBoxSprite, 17, 17, WHITE); 
            DrawTextEx(font, score,Vector2{28, 20},64, 0, BLACK);        
            DrawTextEx(font, highscore,Vector2{17, 90}, 32, 0, BLACK);        

            EndDrawing();
        }
       
    }

    UnloadTexture(playerSprite);
    UnloadTexture(lavaSprite);
    UnloadTexture(platformSprite);
    UnloadTexture(coinSprite);
    UnloadTexture(scoreBoxSprite);
    UnloadTexture(logo);
    UnloadTexture(splashEggSprite);
    UnloadSound(fxClick);
    UnloadSound(fxLaunch);
    UnloadSound(fxDeath);
    UnloadSound(fxCoin);
    UnloadSound(fxSplash);
    UnloadSound(fxSelect);
    UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();        
   
    return 0;
}
