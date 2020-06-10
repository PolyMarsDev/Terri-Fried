#include "player.h"
#include <math.h>
#include <vector>

const double pi = 3.1415926535897;
const int gravity = 1;
const int screenWidth = 640;
const int screenHeight = 480;

Player::Player(double x, double y, int width, int height)
{
	this->x = x;
	this->y = y;
    this->width = width;
	this->height = height;
    onPlatform = false;
}

double Player::getX()
{
	return x;
}

double Player::getY()
{
	return y;
}

void Player::setX(int x)
{
    this->x = x;
}

void Player::setY(int y)
{
    this->y = y;
}

int Player::getWidth()
{
    return width;
}

int Player::getHeight()
{
    return height;
}

bool Player::isOnGround()
{
    return onPlatform;
}
bool Player::isOnPlatform()
{
    return onPlatform;
}

void Player::setOnPlatform(bool result)
{
    onPlatform = result;
}

void Player::setVelocity(double x, double y)
{
    velocityX = x;
    velocityY = y;
}

double Player::getVelocityX()
{
    return velocityX;
}

double Player::getVelocityY()
{
    return velocityY;
}

void Player::updatePosition()
{
    x += velocityX;
    y += velocityY;
    if (!isOnGround())
    {
        velocityY += gravity;
    } else 
    {
        velocityX = 0;
        velocityY = 0;
    }
    if (x < 0)
    {
        velocityX *= -1;
    }
    if (x + width > screenWidth)
    {
        velocityX *= -1;
    }
}

