// user_MotorCtl.h
#ifndef USER_MOTORCTL_H
#define USER_MOTORCTL_H

#include "main.h"

// Print Color
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RRED     "\033[31m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"


typedef struct{
    double lspeed;
    double rspeed;
} Speed_t;


extern volatile Speed_t measured_speed;

void getSpeed(int time);



#endif