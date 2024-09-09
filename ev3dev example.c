#include "rbc.h"

const float MSPM = 0;

void move(int speed, float metres) {
    setMotorSpeed(motorA, speed);
    setMotorSpeed(motorB, speed);
    
    delay(metres*MSPM);

    setMotorSpeed(motorA, 0);
    setMotorSpeed(motorB, 0);
}

task main() {
    //example code
    move(100, 5000)
}