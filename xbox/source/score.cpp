#include "score.h"

#include <stdio.h>
#include <stdlib.h>

int getHighscore();

int scoreInt = 0;
int highscoreInt = getHighscore();
char score[32];
char highscore[32];

int getHighscore() {
    FILE *scorefile = fopen("D:\\highscore.bin", "rb");
    
    if(!scorefile)
        return 0;
    
    uint32_t val;
    fread(&val, sizeof(uint32_t), 1, scorefile);
    
    fclose(scorefile);
    return val;
}

void saveHighscore() {
    FILE *scorefile = fopen("D:\\highscore.bin", "wb");
    
    fwrite(&highscoreInt, sizeof(uint32_t), 1, scorefile);
    
    fclose(scorefile);
}

void resetScore() {
    scoreInt = 0;
    sprintf(score, "%03d", scoreInt);
    saveHighscore();
}

void addScore(int amount) {
    scoreInt += amount;
    sprintf(score, "%03d", scoreInt);
    
    if(scoreInt > highscoreInt) {
        highscoreInt = scoreInt;
        sprintf(highscore, "BEST: %d", highscoreInt);
    }
}
