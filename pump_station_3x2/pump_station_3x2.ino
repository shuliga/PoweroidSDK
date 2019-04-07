
#define ID "PWR-PMS-32"

#include <SoftwareSerial.h>
#include <Wire.h>
#include <../Poweroid_SDK_10/src/global.h>
#include <../Poweroid_SDK_10/src/Poweroid10.h>
#include "pump_station_3x2_state.h"
#include "pump_station_3x2_prop.h"
#include <../Poweroid_SDK_10/src//ultrasonic.h>
#include <../Poweroid_SDK_10/lib/MultiClick/MultiClick.h>
#include <../Poweroid_SDK_10/lib/DS1307/DS1307.h>


Timings timings = {0};
unsigned long SBY_MILLS = 0L;
TimingState FLASH(750L);
TimingState FLASH_SBY(250L);

#define IND IND_3

MultiClick btn = MultiClick(IN3_PIN);

Context CTX = Context(SIGNATURE, FULL_VERSION, PROPS.FACTORY, PROPS.props_size, ID,
                      PROPS.DEFAULT_PROPERTY);

Commander CMD(CTX);
Bt BT(CTX.id);

#ifndef NO_CONTROLLER
Controller CTRL(CTX, CMD);
Pwr PWR(CTX, &CMD, &CTRL, &BT);
#else
Pwr PWR(CTX, &CMD, NULL, &BT);
#endif

const char * BANNER_FMT = "%02d:%02d:%02d";

void apply_timings() {
    timings.countdown_power.interval = (unsigned long) PROPS.FACTORY[0].runtime * 3600000L +
                                      (unsigned long) PROPS.FACTORY[1].runtime * 60000L;
    SBY_MILLS = static_cast<unsigned long>(PROPS.FACTORY[2].runtime * 60000L);
}

uint16_t banner_value;

void fillBanner() {
    bool countDown = state_power != SP_OFF && state_power != SP_DISARM;
    uint16_t totalToGo = static_cast<uint16_t>(countDown ? timings.countdown_power.millsToGo() / 1000 : 0);
    uint8_t hrsToGo = static_cast<uint8_t>(totalToGo / 3600);
    uint16_t secToGoM = totalToGo - (hrsToGo * 3600);
    uint8_t minToGo = static_cast<uint8_t>(secToGoM / 60);
    uint8_t secToGo = static_cast<uint8_t>(secToGoM - (minToGo * 60));
    if (countDown){
        sprintf(BANNER, BANNER_FMT , hrsToGo, minToGo, secToGo);
    } else {
//        sprintf(BANNER, BANNER_FMT, RTC.get(DS1307_HR, true), RTC.get(DS1307_MIN, false), RTC.get(DS1307_SEC, false));
//        sprintf(BANNER, "L=%dcm", ULTRASONIC.getDistance());
//        sprintf(BANNER, "P=%dkPa", PWR.SENS->getNormalizedSensor(1, -100, 0, 102, 920));
//        int16_t val = PWR.SENS->getNormalizedSensor(1, -100, 0, 102, 920);
//        *BANNER = val;
//        sprintf(BANNER + 2, "%dkPa", val);
        BANNER[0] = 0;
    };
}

void run_state_power(McEvent event) {
    switch (state_power) {
        case SP_OFF: {
            prev_state_power = SP_OFF;
            if (event == HOLD) {
                state_power = SP_POWER;
            }
            break;
        }
        case SP_POWER: {
            if (prev_state_power == SP_OFF) {
                timings.countdown_power.reset();
            }
            prev_state_power = SP_POWER;
            if (event == HOLD) {
                state_power = SP_OFF;
                break;
            }
            if (event == CLICK) {
                state_power = SP_POWER_SBY;
                break;
            }
            if (!timings.countdown_power.countdown(true, false, false)) {
                state_power = SP_OFF;
                break;
            }
            if (timings.countdown_power.millsToGo() < SBY_MILLS) {
                state_power = SP_POWER_END;
                break;
            }
            break;
        }
        case SP_POWER_SBY: {
            timings.countdown_power.countdown(true, true, false);
            if (event == CLICK) {
                state_power = prev_state_power;
                break;
            }
            if (event == HOLD) {
                state_power = SP_OFF;
                break;
            }
            break;
        }
        case SP_POWER_END: {
            prev_state_power = SP_POWER_END;
            if (event == HOLD) {
                state_power = SP_OFF;
                break;
            }
            if (event == CLICK) {
                state_power = SP_POWER_SBY;
                break;
            }
            if (!timings.countdown_power.countdown(true, false, false)) {
                state_power = SP_OFF;
                break;
            }
            break;
        }
        case SP_DISARM: {
            prev_state_power = SP_DISARM;
            break;
        }
    }
    CMD.printChangedState(prev_state_power, state_power, 0);
}

void setup() {
    PWR.begin();
    ULTRASONIC.begin();
//    CTX.gaugeMode = true;
}

void loop() {

    apply_timings();

    PWR.run();

    run_state_power(btn.checkButton());

    bool power = (state_power == SP_POWER || state_power == SP_POWER_END);

    PWR.power(REL_A, power);
    PWR.power(REL_B, power);

    if (power) {
        if (state_power == SP_POWER) {
            INDICATORS.set(IND, true);
        } else {
            INDICATORS.flash(IND, &FLASH, true);
        }

    } else {
        if (state_power == SP_POWER_SBY) {
            INDICATORS.flash(IND, &FLASH_SBY, true);
        } else {
            INDICATORS.set(IND, false);
        }
    }
}