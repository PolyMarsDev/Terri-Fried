#include "player.h"

#include <math.h>

const int gravity = 1;

const int screenWidth = 640;
const int screenHeight = 480;

Player::Player(double x, double y, int width, int height) {
	this->x = x;
	this->y = y;
    this->width = width;
	this->height = height;
    isOnPlatform = false;
}

void Player::updatePosition() {
    x += velocityX;
    y += velocityY;
    if (!isOnPlatform) {
        velocityY += gravity;
    }
    else {
        velocityX = 0;
        velocityY = 0;
    }
    if (x < 0) {
        velocityX *= -1;
    }
    if (x + width > screenWidth) {
        velocityX *= -1;
    }
}
