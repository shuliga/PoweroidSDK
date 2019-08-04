
#define ID "PWR-PMS-32"

#include <SoftwareSerial.h>
#include <Wire.h>
#include <../Poweroid_SDK_10/src/global.h>
#include <../Poweroid_SDK_10/src/Poweroid10.h>
#include "poweroid_tensiometer_1x1_state.h"
#include "poweroid_tensiometer_1x1_prop.h"
#include <../Poweroid_SDK_10/lib/MultiClick/MultiClick.h>
#include <../Poweroid_SDK_10/lib/DS1307/DS1307.h>


Timings timings = {0, 0};

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

void apply_timings() {
    timings.fill_time.interval = (unsigned long) PROPS.FACTORY[2].runtime;
    timings.alarm_time.interval = (unsigned long) PROPS.FACTORY[3].runtime;
}

static uint16_t pressure;
static uint16_t pressure_min;
static uint16_t pressure_max;

void processSensors() {
    pressure = PWR.SENS->getNormalizedSensor(SEN_2, -100, 0, 102, 920);
    pressure_min = PROPS.FACTORY[0].runtime;
    pressure_max = PROPS.FACTORY[1].runtime;
}

void fillBanner() {
    processSensors();
    if (state_valve == SP_ALARM_SHUT) {
        BANNER.mode = 0;
        sprintf(BANNER.data.text, "%s", "ALARM");
    } else {
        BANNER.mode = 1;
        BANNER.data.gauges[0].val = pressure;
        BANNER.data.gauges[0].min = pressure_min;
        BANNER.data.gauges[0].max = pressure_max;
        BANNER.data.gauges[0].g_min = PROPS.FACTORY[0].minv / PROPS.FACTORY[0].scale;
        BANNER.data.gauges[0].g_max = PROPS.FACTORY[1].maxv / PROPS.FACTORY[1].scale;
        BANNER.data.gauges[0].measure = KPA;

    };
}

void run_state_power() {
    switch (state_valve) {
        case SV_READY: {
            if (pressure < pressure_min) {
                prev_state_power = SV_READY;
                state_valve = SV_OPEN;
                timings.fill_time.reset();
            }
            break;
        }
        case SV_OPEN: {
            prev_state_power = SV_OPEN;
            if (timings.fill_time.isTimeAfter(true)) {
                if (pressure < pressure_min) {
                    state_valve = SP_OPEN_ALARM;
                    timings.alarm_time.reset();
                } else {
                    state_valve = SV_READY;
                }
            } else {
                if (pressure > pressure_max) {
                    state_valve = SV_READY;
                    timings.alarm_time.reset();
                }
            }
            break;
        }
        case SP_OPEN_ALARM: {
            prev_state_power = SP_OPEN_ALARM;
            if (pressure >= pressure_min) {
                state_valve = SV_OPEN;
                timings.fill_time.reset();
            } else {
                if (timings.alarm_time.isTimeAfter(true)) {
                    state_valve = SP_ALARM_SHUT;
                }
            }
            break;
        }
        case SP_ALARM_SHUT: {
            prev_state_power = SP_ALARM_SHUT;
            if (pressure >= pressure_min) {
                state_valve = SV_READY;
            }
            break;
        }
        case SV_DISARM: {
            prev_state_power = SV_DISARM;
            break;
        }
    }
    CMD.printChangedState(prev_state_power, state_valve, 0);
}

void setup() {
    PWR.begin();
}

void loop() {

    apply_timings();

    PWR.run();

    run_state_power();

    bool power = (state_valve == SP_OPEN_ALARM || state_valve == SV_OPEN);

    PWR.power(REL_A, power);

}
