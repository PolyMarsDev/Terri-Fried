#ifndef PLATFORM_H
#define PLATFORM_H

class Platform {
private:
    double x;
    double y;
    int width;
    int height;
    bool hasCoin;
    int coinX;
    int coinY;
    
public:
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
};
#endif