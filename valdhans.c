#include "rbc.h"
#include <stdio.h>
//n
//change these values as needed
const float wheelSize = 2.3; //in cm
const float wheelBase = 18.5; //in cm
const float clearance = 12.5; //i think those are cm but im not sure
const int useAlternate = 0 ; //whether to use alternative encoder for rotating (1 - enabled, 0 - disabled)

const int maxValueS1 = 44;
const int minValueS1 = 6;
const int maxValueS2 = 34;
const int minValueS2 = 4;

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

float normalize(float value, float maxValue, float minValue) {
    return (value-minValue)/(maxValue-minValue);
}

void move(float cm, int power) {
    long startB = getMotorEncoder(motorA);

    long degreesNeeded = startB + distanceToDegrees(cm);
    long currentB = startB;

    setMotorSpeed(motorA, power);
    setMotorSpeed(motorB, power);

    int powerUsed = power;
    while (true) {
        currentB = getMotorEncoder(motorA);
        
        displayTextLine(1, "degreesNeeded=%lg", degreesNeeded);
        displayTextLine(2, "currentB=%lg", currentB);

        if (currentB + (360*1.5) >= degreesNeeded && power >= 50) { powerUsed = 50; }
        if (currentB + 360 >= degreesNeeded && power >= 30) { powerUsed = 30; }
        if (currentB + 90 >= degreesNeeded && power >= 10) { powerUsed = 10; }
        if (currentB >= degreesNeeded) { powerUsed = 0; }
        
        setMotorSpeed(motorA, powerUsed);
        setMotorSpeed(motorB, powerUsed);

        float difference = getMotorEncoder(motorA) - getMotorEncoder(motorB);
        if (difference > 2) {
            setMotorSpeed(motorB, powerUsed-2);
        }
        if (difference < -2) {
            setMotorSpeed(motorA, powerUsed-2);
        }

        

        if (powerUsed == 0) {
            break;
        }

        delay(10);
    }   
}

void infMove(int power) {
    int powerUsed = power;
    int distance;
    while (true) {
        distance = getUSDistance(S2);
    
        if (distance <= clearance) {
            powerUsed = 0;
        } 

        if (distance + 30 <= clearance && power >= 50) {
            powerUsed = 50;
        }

        if (distance + 10 <= clearance && power >= 20) {
            powerUsed = 20;
        }

        setMotorSpeed(motorA, powerUsed);
        setMotorSpeed(motorB, powerUsed);

        displayTextLine(1, "distance: %i", distance);
        displayTextLine(2, "pwr: %i", powerUsed);

        if (powerUsed == 0) {
            break;
        }
    }
}

float rotate(float degrees) {
    long startB = getMotorEncoder(motorA);
    long startC = getMotorEncoder(motorB);

    if (useAlternate) {
        startB = getMotorEncoder(motorB);
        startC = getMotorEncoder(motorA);
    }

    long distance = distanceToDegrees(((2*PI*turnRadius)/360)*degrees);

    long degreesNeededB = startB + distance;
    long degreesNeededC = startC - distance;
    long currentB = startB;
    long currentC = startC;

    int fwdB = degreesNeededB > startB;
    int mod = 1;
    if (!fwdB) { mod = -1; } 

    setMotorSpeed(motorA, 50*mod);
    setMotorSpeed(motorB, 50*-mod);

    while (true) {
        currentB = getMotorEncoder(motorA);

        if ((currentB + 180 >= degreesNeededB && fwdB) || (currentB + 90 <= degreesNeededB && !fwdB)) {
            setMotorSpeed(motorA, 20*mod);
            setMotorSpeed(motorB, 20*-mod);
        }


        if ((currentB >= degreesNeededB && fwdB) || (currentB <= degreesNeededB && !fwdB)) {
            setMotorSpeed(motorA, 0);
            setMotorSpeed(motorB, 0);
            break;
        }



        delay(10);
    }

    if (getMotorEncoder(motorB) > startC) {
        return getMotorEncoder(motorB) - startC;
    }
    return getMotorEncoder(motorA) - startB;
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
    resetMotorEncoder(motorA);
    resetMotorEncoder(motorB);
    
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

    int travelled = degreesToDistance(getMotorEncoder(motorA)-turnDifference);
    displayTextLine(1, "travelled: %icm", travelled);
    displayTextLine(2, "estimated: %icm", (lenX+lenY)*loops*2);
    delay(10000);
}

void objectEvasionSystem1337(int power) {
    float rightDistance = -1;
    float leftDistance = -1;

    while (true) {
        float distance = getUSDistance(S2);
        displayTextLine(1, "distance=%f", distance);

        setMotorSpeed(motorA, power);
        setMotorSpeed(motorB, power);

        float difference = getMotorEncoder(motorA) - getMotorEncoder(motorB);
        if (difference > 2) {
            setMotorSpeed(motorB, power-2);
        }
        if (difference < -2) {
            setMotorSpeed(motorA, power-2);
        }
    
        if (distance <= clearance || getTouchValue(S1)) {
            setMotorSpeed(motorA, 0);
            setMotorSpeed(motorB, 0);
        
            rotate(-90);
            leftDistance = getUSDistance(S2);
            rotate(180);
            rightDistance = getUSDistance(S2);

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

void mazeSolver(int walls) {
    int finalPhase = 0;
    int currentWall = 0;

    int holePos[walls];
    int currentPos = 0;

    while (true) {
        if (!finalPhase) {
            rotate(90);

            if (getUSDistance(S2) > clearance*3) {
                finalPhase = 1;
                holePos[currentWall] = currentPos;
                currentPos = 0;
                continue;
            }

            rotate(-90);
            move(30, 100);
            currentPos++;
        }

        if (finalPhase) {
            infMove(100);
            rotate(90);
            infMove(100);
            
            //break;
            rotate(180);
            currentWall++;

            if (currentWall >= walls) {
                break;
            }

            finalPhase = 0;
        }
    }
}

task pathFinder(int power) {
    while (true) {
        float rs1 = getColorReflected(S1);
        float rs2 = getColorReflected(S2);

        int speedA = power*normalize(rs1, maxValueS1, minValueS1);
        int speedB = power*normalize(rs2, maxValueS2, minValueS2);

        setMotorSpeed(motorA, speedA);
        setMotorSpeed(motorB, speedB);

        displayTextLine(1, "Speed MAX=%i A=%i B=%i", power, speedA, speedB);
        displayTextLine(2, "RS S1=%.1f S2=%.1f", rs1, rs2);

        delay(50);
    }
}

task watchPFValues() {
    while (true) {
        float rs1 = getColorReflected(S1);
        float rs2 = getColorReflected(S2);

        displayTextLine(1, "S1=%.0f", rs1);
        displayTextLine(2, "S2=%.0f", rs2);
        displayTextLine(3, "n1=%.3f", normalize(rs1, maxValueS1, minValueS1));
        displayTextLine(4, "n2=%.3f", normalize(rs2, maxValueS2, minValueS2));

        delay(50);
    }
}

task main() {
    //do not remove
    wheelPerimeter = 2*PI*wheelSize;
    turnRadius = wheelBase/2;
    //---------------

    //watchPFValues();

    pathFinder(50);
}

