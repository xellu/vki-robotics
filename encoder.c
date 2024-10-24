#include "rbc.h"
#include <stdio.h>

//change these values as needed
const float wheelSize = 2.3; //in cm
const float wheelBase = 18.9; //in cm
const float clearance = 12.5; //i think those are cm but im not sure
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
    setMotorSpeed(motorD, power);

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
        setMotorSpeed(motorD, powerUsed);

        float difference = getMotorEncoder(motorB) - getMotorEncoder(motorD);
        if (difference > 2) {
            setMotorSpeed(motorD, powerUsed-2);
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
    long startC = getMotorEncoder(motorD);
    long distance = distanceToDegrees(((2*PI*turnRadius)/360)*degrees);

    long degreesNeededB = startB + distance;
    long currentB = startB;

    int fwdB = degreesNeededB > startB;
    int mod = 1;
    if (!fwdB) { mod = -1; } 

    setMotorSpeed(motorB, 50*mod);
    setMotorSpeed(motorD, 50*-mod);

    while (true) {
        currentB = getMotorEncoder(motorB);

        if ((currentB + 90 >= degreesNeededB && fwdB) || (currentB + 90 <= degreesNeededB && !fwdB)) {
            setMotorSpeed(motorB, 20*mod);
            setMotorSpeed(motorD, 20*-mod);
        }


        if ((currentB >= degreesNeededB && fwdB) || (currentB <= degreesNeededB && !fwdB)) {
            setMotorSpeed(motorB, 0);
            setMotorSpeed(motorD, 0);
            break;
        }

        delay(10);
    }

    if (getMotorEncoder(motorD) > startC) {
        return getMotorEncoder(motorD) - startC;
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
    resetMotorEncoder(motorD);
    
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

void pathFinder(int power) {
    float rightDistance = -1;
    float leftDistance = -1;

    while (true) {
        float distance = getUSDistance(S1);
        displayTextLine(1, "distance=%f", distance);

        setMotorSpeed(motorB, power);
        setMotorSpeed(motorD, power);
    
        if (distance <= clearance || getTouchValue(S2)) {
            setMotorSpeed(motorB, 0);
            setMotorSpeed(motorD, 0);
        
            rotate(-90);
            leftDistance = getUSDistance(S1);
            rotate(180);
            rightDistance = getUSDistance(S1);

            if (rightDistance < clearance && leftDistance < clearance) {
                rotate(90);
            }

            if (rightDistance < leftDistance) {
                rotate(-180);
            }


        }

        delay(10);
    }
}

void wallBypass1337() {
    int finalPhase = 0;
    int moveMultiplier = 0;

    while (true) {
        if (!finalPhase) {
            move(30, 100);
            rotate(90);

            moveMultiplier++;
            
            if (getUSDistance(S1) > clearance*3) {
                finalPhase = 1;
                continue;
            }

            rotate(-90);
        }

        if (finalPhase) {
            move(30, 50);
            rotate(90);
            move(30*moveMultiplier, 100);
            
            break;
        }
    }
}

task main() {
    //do not remove
    wheelPerimeter = 2*PI*wheelSize;
    turnRadius = wheelBase/2;
    //-------------

    wallBypass1337();
}