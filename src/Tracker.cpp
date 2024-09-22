#include "Tracker.h"

Tracker::Tracker(StepMotor *motor, int azimPulsPerAngle, bool directionInversion)
    : _azimutMotor(motor), _azimPulsPerAngle(azimPulsPerAngle), _directionInversion(directionInversion) {}

void Tracker::online() {
    _azimutMotor->enable();
}
void Tracker::offline(){
    _azimutMotor->disable();
}

void Tracker::turnAzimut(int16_t angle) {
    int16_t pulses = abs(angle) * _azimPulsPerAngle;
    TurnDirection dir;

    if (angle > 0) {
        if (_directionInversion) {dir = TurnDirection::Minus;}
        else {dir = TurnDirection::Plus;}
    }
    else {
        if (_directionInversion) {dir = TurnDirection::Plus;}
        else {dir = TurnDirection::Minus;}
    }

    _azimutMotor->turn(pulses, dir);
}
