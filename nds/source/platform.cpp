#include "platform.h"
#include <math.h>
const int screenWidth = 256;
const int screenHeight = 192;
Platform::Platform(int index)
{
    width = 50;
    height = 16;
    x = rand()%196  + 10;
    y = 0 - height - (index * 50);
    int coinInt = rand()% 4;
    if (coinInt == 0 || index == 0)
    {
        hasCoin = false;
    } else {
        hasCoin = true;
    }
    coinX = x + (width/2) - 6;
    coinY = y - 12 - 3;
    
}

double Platform::getX()
{
    return x;
}

double Platform::getY()
{
    return y;
}

int Platform::getWidth()
{
    return width;
}

int Platform::getHeight()
{
    return height;
}

bool Platform::getHasCoin()
{
    return hasCoin;
}
void Platform::setHasCoin(bool value)
{
    hasCoin = value;
}
int Platform::getCoinX()
{
    return coinX;
}
int Platform::getCoinY()
{
    return coinY;
}

void Platform::updatePosition()
{
    y+=0.5;
    coinX = x + (width/2) - 6;
    coinY = y - 12 - 3;
    if (y > screenHeight)
    {
        x = rand()%196  + 10;
        y = 0 - height;
        int coinInt = rand()% 4;
        if (coinInt == 0)
        {
            hasCoin = false;
        } else {
            hasCoin = true;
        }
    }
}

void Platform::resetPosition(int index)
{
    x = rand()%196  + 10;
    y = 0 - height - (index * 50);
    int coinInt = rand()% 4;
    if (coinInt == 0 || index == 0)
    {
        hasCoin = false;
    } else {
        hasCoin = true;
    }
    coinX = x + (width/2) - 6;
    coinY = y - 12 - 3;
}