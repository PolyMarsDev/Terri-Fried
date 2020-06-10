#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
class Player
{
  private:
	double x;
    double y;
    int width;
    int height;
    bool onPlatform;
    double velocityX;
    double velocityY;
	
  public:
	Player(double x, double y, int width, int height);
	double getX();
	double getY();
    void setX(int x);
    void setY(int y);
    int getWidth();
    int getHeight();
    bool isOnGround();
    bool isOnPlatform();
    void setOnPlatform(bool result);
    void setVelocity(double x, double y);
    double getVelocityX();
    double getVelocityY();
    void updatePosition();

};
#endif