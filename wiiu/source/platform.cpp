#include "platform.h"
#include <math.h>

const int screenWidth = 1280;
const int screenHeight = 720;

Platform::Platform(int index) {
    width = 100;
    height = 32;
    
    x = rand()% 1140 + 20;
    y = 0 - height - (index * 135);
    
    int coinInt = rand()% 4;
    
    if (coinInt == 0 || index == 0)
        hasCoin = false;
    else
        hasCoin = true;
    
    coinX = x + width/2 - 24/2;
    coinY = y - 24 - 5;
}

double Platform::getX() {
    return x;
}

double Platform::getY() {
    return y;
}

int Platform::getWidth() {
    return width;
}

int Platform::getHeight() {
    return height;
}

bool Platform::getHasCoin() {
    return hasCoin;
}

void Platform::setHasCoin(bool value) {
    hasCoin = value;
}

int Platform::getCoinX() {
    return coinX;
}

int Platform::getCoinY() {
    return coinY;
}

void Platform::updatePosition() {
    y+=1;
    
    coinX = x + width/2 - 24/2;
    coinY = y - 24 - 5;
    
    if (y > screenHeight) {
        x = rand()% 660 + 20;
        y = 0 - height;
        
        int coinInt = rand()% 4;
        
        if (coinInt == 0)
            hasCoin = false;
        else
            hasCoin = true;
    }
}
