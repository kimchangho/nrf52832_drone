#ifndef MOTOR_H__
#define MOTOR_H__

#include "main.h"

#define PWM_TOP 800 // count to 800 / 20khz

typedef struct {
    float motor1;
    float motor2;
    float motor3;
    float motor4;
} motor_values;

void motor_init(void);
void motor_values_update(motor_values values);

#endif
