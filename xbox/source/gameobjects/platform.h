#ifndef PLATFORM_H
#define PLATFORM_H

class Platform {
public:
    double x;
    double y;
    int width;
    int height;
    bool hasCoin;
    int coinX;
    int coinY;
    
    Platform(int index);
    double getX();
    double getY();
    int getWidth();
    int getHeight();
    bool getHasCoin();
    void setHasCoin(bool value);
    int getCoinX();
    int getCoinY();
    void updatePosition();
    void resetPosition(int index);
};
#endif
