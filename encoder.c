#include "rbc.h"
#include <stdio.h>

//change these values as needed
const float wheelSize = 2.3; //in cm
const float wheelBase = 18.9; //in cm
//---------

float wheelPerimeter = 0;
float turnRadius = 0;

float posX = 0;
float posY = 0;
float yaw = 0;

float distanceToDegrees(float distance) {
    return (distance/wheelPerimeter)*360;
}

float degreesToDistance(float degrees) {
    return (degrees/360)*wheelPerimeter;
}

float rad2deg(float rads) {
    return rads*(180/PI);
}

int degreeFix(int degrees) {
    if (degrees >= 180) {
        degrees = (degrees - 180) * -1;
    }

    if (degrees >= 360) {
        degrees = degrees - 360;
    }
    if (-degrees >= 360) {
        degrees = degrees + 360;
    }
    return degrees;
}

void move(float cm, int power) {
    long startB = getMotorEncoder(motorB);

    long degreesNeeded = startB + distanceToDegrees(cm);
    long currentB = startB;

    setMotorSpeed(motorB, power);
    setMotorSpeed(motorC, power);

    int powerUsed = power;
    while (true) {
        currentB = getMotorEncoder(motorB);
        
        displayTextLine(1, "degreesNeeded=%lg", degreesNeeded);
        displayTextLine(2, "currentB=%lg", currentB);

        if (currentB + (360*1.5) >= degreesNeeded && power >= 50) { powerUsed = 50; }
        if (currentB + 360 >= degreesNeeded && power >= 30) { powerUsed = 30; }
        if (currentB + 90 >= degreesNeeded && power >= 10) { powerUsed = 10; }
        if (currentB >= degreesNeeded) { powerUsed = 0; }
        
        setMotorSpeed(motorB, powerUsed);
        setMotorSpeed(motorC, powerUsed);

        float difference = getMotorEncoder(motorB) - getMotorEncoder(motorC);
        if (difference > 2) {
            setMotorSpeed(motorC, powerUsed-2);
        }
        if (difference < -2) {
            setMotorSpeed(motorB, powerUsed-2);
        }

        

        if (powerUsed == 0) {
            break;
        }

        delay(10);
    }   
}

float rotate(float degrees) {
    long startB = getMotorEncoder(motorB);
    long startC = getMotorEncoder(motorC);
    long distance = distanceToDegrees(((2*PI*turnRadius)/360)*degrees);

    long degreesNeededB = startB + distance;
    long currentB = startB;

    int fwdB = degreesNeededB > startB;
    int mod = 1;
    if (!fwdB) { mod = -1; } 

    setMotorSpeed(motorB, 50*mod);
    setMotorSpeed(motorC, 50*-mod);

    while (true) {
        currentB = getMotorEncoder(motorB);

        if ((currentB + 90 >= degreesNeededB && fwdB) || (currentB + 90 <= degreesNeededB && !fwdB)) {
            setMotorSpeed(motorB, 20*mod);
            setMotorSpeed(motorC, 20*-mod);
        }


        if ((currentB >= degreesNeededB && fwdB) || (currentB <= degreesNeededB && !fwdB)) {
            setMotorSpeed(motorB, 0);
            setMotorSpeed(motorC, 0);
            break;
        }

        delay(10);
    }

    if (getMotorEncoder(motorC) > startC) {
        return getMotorEncoder(motorC) - startC;
    }
    return getMotorEncoder(motorB) - startB;
}

void moveXY(float x, float y, int power) {
    x = x - posX;
    y = y - posY;

    int degrees = (int) rad2deg(atan2f(x, y)) - yaw;
    degrees = degreeFix(degrees);

    float distance = sqrt(x*x + y*y);

    rotate(degrees);
    move(distance, power);

    posX = posX + x;
    posY = posY + y;
    yaw = yaw + degrees;
    if (yaw >= 360) {
        yaw = yaw - 360;
    }
}

void setYaw(int degrees) {
    degrees = yaw-360+degrees;
    degrees = degreeFix(degrees);

    rotate(degrees);

    yaw = yaw + degrees;
    if (yaw >= 360) {
        yaw = yaw - 360;
    }
}

void tablePerimeter(int loops) {
    resetMotorEncoder(motorB);
    resetMotorEncoder(motorC);
    
    const int lenY = 210;
    const int lenX = 90;

    int i;
    float turnDifference = 0;
    for (i = 0; i < loops*2; i++) {
        move(lenY, 90);
        turnDifference = turnDifference + rotate(90);
        move(lenX, 90);
        turnDifference = turnDifference + rotate(90);
    }

    int travelled = degreesToDistance(getMotorEncoder(motorB)-turnDifference);
    displayTextLine(1, "travelled: %icm", travelled);
    displayTextLine(2, "estimated: %icm", (lenX+lenY)*loops*2);
    delay(10000);
}

task main() {
    //do not remove
    wheelPerimeter = 2*PI*wheelSize;
    turnRadius = wheelBase/2;
    //-------------
    /*
    moveXY(-17, 30, 100);
    moveXY(28, 38, 100);
    moveXY(0, 0, 100);

    setYaw(0);
    */
   tablePerimeter(1);
}