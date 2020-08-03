#ifndef __SCORE_H__
#define __SCORE_H__

extern int highscoreInt;
extern char score[32];
extern char highscore[32];

void resetScore();
void addScore(int amount);

#endif
