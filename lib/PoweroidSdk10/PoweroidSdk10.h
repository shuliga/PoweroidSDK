#ifndef POWEROID_SDK_10_H
#define POWEROID_SDK_10_H

#define VERSION "POWEROID SDK 1.0"
#define PWR21

#include "timings.h"
#include "commons.h"
#include "pin_io.h"
#include "sensors.h"

static char SIGNATURE[] = "PWR";

const long INST_DELAY = 1000L;

const uint8_t LED_PIN = 13;
const uint8_t LED3_PIN = 7;
const uint8_t LED2_PIN = 8;
const uint8_t LED1_PIN = 9;
const uint8_t LED4_PIN = 12;
const uint8_t LED5_PIN = 13;

const uint8_t PWR1_PIN = 10;
const uint8_t PWR2_PIN = 11;

const uint8_t LED_PINS[] = {LED1_PIN, LED2_PIN, LED3_PIN};

const uint8_t OUT_PINS[] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN, LED5_PIN, PWR1_PIN, PWR2_PIN};

const int del = 333;

static String states[] = {"OFF", "OFF"};

static TimingState flash_333;
static TimingState hold_on[3];

void init_outputs();
void init_inputs();
void init_system();
bool checkInstalled(int pin, bool inst);
bool checkInstalledWithDelay(int pin, bool inst, TimingState *hold_on);
void init_sensors();
void check_installed();
bool is_sensor_on(int index);
int get_sensor_val(int index);
bool is_sensor_val(int index, int val);
void indicate();

void printVersion();

#endif
