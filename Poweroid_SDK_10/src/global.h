//
// Created by SHL on 18.11.2017.
//
#include <Arduino.h>

#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef POWEROID_SDK_10_H
    #define EXTERN
#else
    #define EXTERN extern
#endif

#define BUFF_SIZE 65
#define LINE_SIZE 16

EXTERN char BUFF[BUFF_SIZE];
EXTERN char BANNER[LINE_SIZE];
EXTERN uint8_t PWR_FLAGS;


#endif //GLOBAL_H