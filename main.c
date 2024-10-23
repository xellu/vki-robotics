#include "rbc.h"
#include <stdio.h>

const int distanceUnit = 3030; //<-- (1000/0.165)/2 (miliseconds/meters)/2
const int degreeUnit = 370; //<-- degrees/time - dont remember the numbers
float posX = 0;
float posY = 0;
float yaw = 0;

void move(float distance, int power) {
    delay(150);

    int powerMod = power;
    if (powerMod < 0) { powerMod = powerMod * -1; }

    setMotorSpeed(motorB, power);
    setMotorSpeed(motorC, power);

    delay(distance*distanceUnit*(100/powerMod));

    setMotorSpeed(motorB, 0);
    setMotorSpeed(motorC, 0);
}

void rotate(float degrees) {
    delay(150);

    int modifier = 1;
    if (degrees < 0) { modifier = -1; }

    setMotorSpeed(motorB, 50*modifier);
    setMotorSpeed(motorC, -50*modifier);

    delay(degreeUnit*(degrees*modifier/90)*2);

    setMotorSpeed(motorB, 0);
    setMotorSpeed(motorC, 0);
}

float rad2deg(float rads) {
    return rads*(180/3.141);
}

void moveXY(float x, float y, int power) {
    x = x - posX;
    y = y - posY;

    int degrees = (int) rad2deg(atan2f(x, y)) - yaw;
    float distance = sqrt(x*x + y*y);

    rotate(degrees);
    move(distance, power);

    posX = posX + x;
    posY = posY + y;
    yaw = degrees;
    //printf("new pos: %f, %f", posX, posY);
}

task main() {
    moveXY(0.1, 0.1, 100);
    moveXY(0, 0, 50);
}