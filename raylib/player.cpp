#include "raylib.h"
#include "player.h"
#include <math.h>
#include <vector>

const double pi = 3.1415926535897;
const int gravity = 1;
const int screenWidth = 800;
const int screenHeight = 450;

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
    velocity = (Vector2){static_cast<float>(x), static_cast<float>(y)};
}

Vector2 Player::getVelocity()
{
    return velocity;
}

void Player::updatePosition()
{
    x += velocity.x;
    y += velocity.y;
    if (!isOnGround())
    {
        velocity.y += gravity;
    } else 
    {
        velocity = (Vector2){0, 0};
    }
    if (x < 0)
    {
        velocity.x *= -1;
    }
    if (x + width > 800)
    {
        velocity.x *= -1;
    }
}

