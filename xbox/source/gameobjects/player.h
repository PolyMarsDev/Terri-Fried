#ifndef PLAYER_H
#define PLAYER_H

class Player {
public:
	double x;
    double y;
    
    int width;
    int height;
    
    bool isOnPlatform;
    
    double velocityX;
    double velocityY;
	
	Player(double x, double y, int width, int height);
    void updatePosition();
};

#endif
