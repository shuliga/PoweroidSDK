//
// Created by SHL on 20.03.2017.
//

#define INCR(val, max) val < (max) ? (val)++ : val
#define DECR(val, min) val > (min) ? (val)-- : val

#include "global.h"
#include <Rotary.h>
#include <ACROBOTIC_SSD1306.h>
#include "controller.h"

#ifndef NO_CONTROLLER

#define DISPLAY_BASE 1
#define DISPLAY_BOTTOM 7

static enum State {
    EDIT_PROP, BROWSE, STORE, SLEEP, STATES, SUSPEND
} oldState = STORE, state = BROWSE;

static TimingState sleep_timer = TimingState(100000L);
static TimingState autoComplete_timer = TimingState(6000L);

static volatile bool control_touched = false;
static volatile uint8_t prop_idx = 0;
static volatile uint8_t state_idx = 0;
static volatile long prop_value;

static volatile int props_idx_max = 0;
static volatile int state_idx_max = 0;


static Property remoteProperty;

static int8_t c_prop_idx = -1;
static int8_t c_state_idx = -1;
static long c_prop_value = -1;
static long old_prop_value;
static bool dither = false;

static volatile long prop_min;
static volatile long prop_max;
static volatile uint8_t prop_measure;

#if defined(ENC1_PIN) && defined(ENC2_PIN)
Rotary encoder = Rotary(ENC1_PIN, ENC2_PIN);
#endif

MultiClick encoderClick = MultiClick(ENC_BTN_PIN);
TimingState displayTiming = TimingState(1000L);


Controller::Controller(Context &_ctx, Commands &_cmd) : cmd(&_cmd), ctx(&_ctx) {}

void Controller::begin() {
    initDisplay();
#if defined(ENC1_PIN) && defined(ENC2_PIN)
    initEncoderInterrupts();
#endif
}

void Controller::initEncoderInterrupts() {
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
    sei();
    props_idx_max = ctx->props_size - 1;
    state_idx_max = state_count -1;
}

void Controller::initDisplay() {
    if (oled.checkAndInit(FLIP_DISPLAY)) {
        oled.clearDisplay();
        oled.setBrightness(0);
    };
}

void Controller::outputState() const {
    strcpy(BUFF, (const char *) ctx->RELAYS.relStatus());
    padLine(BUFF, 1, 0);
    BUFF[15] = (unsigned char) ((ctx->connected ? 130 : 129) + (ctx-> passive ? 0 : 2));
    oled.setTextXY(0, 0);
    oled.putString(BUFF);
}

void Controller::process() {

    McEvent event = encoderClick.checkButton();

    switch (state) {
        case EDIT_PROP: {

            if (testControl(autoComplete_timer)) {
                loadProperty(prop_idx);
                outputPropDescr(BUFF);
                outputStatus(F("Edit value:"), old_prop_value);
            }

            if (c_prop_value != prop_value || ctx->refreshProps) {
                updateProperty(prop_idx);
                outputPropVal(prop_measure, (int16_t) prop_value, true, true);
            }
            if (event == CLICK || autoComplete_timer.isTimeAfter(true)) {
                goToBrowse();
            };
            if (event == DOUBLE_CLICK) {
                prop_value = old_prop_value;
                updateProperty(prop_idx);
                goToBrowse();
            };
            if (event == HOLD) {
                state = STORE;
            };
            break;
        }
        case BROWSE: {

            if (testControl(sleep_timer) || c_prop_idx != prop_idx || ctx->refreshProps) {
                loadProperty(prop_idx);
                outputPropDescr(BUFF);
                outputPropVal(prop_measure, (int16_t) prop_value, false, true);
                outputStatus(F("Property:"), prop_idx + 1);
            }

            if (event == CLICK) {
                goToEditProp(prop_idx);
            };

            if (event == HOLD || sleep_timer.isTimeAfter(true)) {
                state = SLEEP;
            };

            if (event == DOUBLE_CLICK && !ctx->passive) {
                state = STATES;
            };

            break;
        }
        case STATES: {
            if (testControl(sleep_timer) || c_state_idx != state_idx) {
                strcpy(BUFF, getState(state_idx)->name);
                outputPropDescr(BUFF);
                strcpy(BUFF, getState(state_idx)->state);
                oled.outputTextXY(DISPLAY_BASE + 2, 64, BUFF, true, false);
                outputStatus(F("State:"), state_idx );
                c_state_idx = state_idx;
            }

            if (event == HOLD) {
                disarmState(state_idx, strcmp(getState(state_idx)->state, "DISARM") != 0);
                c_state_idx = -1;
            };

            if (event == CLICK || sleep_timer.isTimeAfter(true)) {
                state = BROWSE;
            };

            break;
        }

        case STORE: {
            firstRun();
            if (!ctx->passive) {
                cmd->storeProps();
            } else {
                Serial.println(cmd->cmd_str.CMD_STORE_PROPS);
                consumeSerial();
            }
            outputStatus(F("<Storing...>"), prop_value);
            delay(500);
            goToBrowse();
            break;
        }
        case SLEEP: {
            if (firstRun()) {
                sleep_timer.reset();
                dither = false;
                switchDisplay(false);
                c_prop_idx = -1; // invalidate cache;
            }


            if (sleep_timer.isTimeAfter(true)) {
                if (dither) {
                    state = SUSPEND;
                } else {
                    dither = true;
                    sleep_timer.reset();
                }

            }

            // Output Sleep Screen
            if (displayTiming.ping() && oled.getConnected()) {
                oled.outputTextXY(DISPLAY_BASE + 2, 64, printDht(), true, dither);
            }

            // Exit SLEEP state on event
            if (event == CLICK) {
                switchDisplay(true);
                state = BROWSE;
            };

            if (control_touched && ctx->defaultPropertyIdx >= 0) {
                control_touched = false;
                switchDisplay(true);
                prop_idx = (uint8_t) ctx->defaultPropertyIdx;
                goToEditProp(prop_idx);
            }

            break;
        }

        case SUSPEND: {
            if (firstRun()) {
                oled.displayOff();
            }

            if (event == CLICK || control_touched) {
                control_touched = false;
                oled.displayOn();
                state = SLEEP;
            };

            break;
        }
    }
    ctx->refreshProps = false;
    if (ctx->refreshState) {
        outputState();
        ctx->refreshState = false;
    }
}

const char *Controller::printDht() const {
    if (ctx->passive){
        Serial.println(cmd->cmd_str.CMD_GET_DHT);
        consumeSerialToBuff();
        return strchr(BUFF, '>');
    } else {
        return ctx->SENS.printDht();
    }
}

void Controller::consumeSerialToBuff() const { BUFF[Serial.readBytesUntil('\n', BUFF, BUFF_SIZE)] = 0; }

bool Controller::testControl(TimingState &timer) const {
    bool fr = firstRun();
    if (fr || control_touched) {
        timer.reset();
        control_touched = false;
        outputState();
    }
    return fr;
}

void Controller::goToBrowse() const {
    c_prop_idx = -1; // invalidate cache
    c_prop_value = -1; // invalidate cache
    state = BROWSE;
}

bool Controller::firstRun() const {
    bool fr = oldState != state;
    oldState = state;
    return fr;
}

void Controller::goToEditProp(uint8_t i) const {
    c_prop_value = -1; // invalidate cache
    c_prop_idx = -1; // invalidate cache
    old_prop_value = prop_value; // for reminder in status
    state = EDIT_PROP;
}

void Controller::loadProperty(uint8_t idx) const {
    if (!ctx->passive) {
        flashStringHelperToChar(ctx->PROPERTIES[idx].desc, BUFF);
        copyProperty(ctx->PROPERTIES[idx], idx);
    } else {
        if (ctx->connected) {
            Serial.print(cmd->cmd_str.CMD_GET_BIN_PROP);
            Serial.println(idx);
            consumeSerialToBuff();
            Serial.readBytes((uint8_t *) &remoteProperty, sizeof(Property));
            copyProperty(remoteProperty, idx);
        }
    }
}

void Controller::copyProperty(Property &prop, uint8_t idx) const {
    cli();
    long scale = prop.scale;
    prop_value = (prop.runtime / scale);
    prop_min = (prop.minv / scale);
    prop_max = (prop.maxv / scale);
    prop_measure = prop.measure;
    c_prop_idx = idx;
    sei();
}

void Controller::updateProperty(uint8_t idx) const {
    if (!ctx->passive) {
        cli();
        ctx->PROPERTIES[idx].runtime = prop_value * ctx->PROPERTIES[idx].scale;
        c_prop_value = prop_value;
        sei();
    } else {
        if (ctx->connected) {
            Serial.print(cmd->cmd_str.CMD_SET_PROP);
            Serial.print(idx);
            Serial.print(':');
            Serial.println(prop_value);
            consumeSerial();
        }
    }
}

void Controller::consumeSerial() const { Serial.readBytesUntil('\n', BUFF, BUFF_SIZE); }

void Controller::switchDisplay(boolean inverse) const {
    oled.displayOff();
    inverse ? oled.setInverseDisplay() : oled.setNormalDisplay();
    oled.clearDisplay();
    oled.displayOn();
}

void Controller::outputPropDescr(char *_buff) {
    oled.setTextXY(DISPLAY_BASE, 0);
    padLine(BUFF, 2, 0);
    oled.putString(_buff);
}

void Controller::outputStatus(const __FlashStringHelper *txt, const long val) {
    flashStringHelperToChar(txt, BUFF);
    oled.setTextXY(DISPLAY_BOTTOM, 0);
    padLine(BUFF, 1, getNumberOfDigits(val));
    oled.putString(BUFF);
    oled.setTextXY(DISPLAY_BOTTOM, (unsigned char) (16 - PROP_SIZE));
    oled.putNumber(val);
}


void inline Controller::padLine(char *_buff, uint8_t lines, uint8_t tail) {
    strncat(_buff, SPACE_BUFF, LINE_SIZE * lines - strlen(_buff) - tail);
}

uint8_t Controller::getNumberOfDigits(long i) {
    return i > 0 ? (uint8_t) log10((double) i) + 1 : 1;
}

void Controller::outputPropVal(uint8_t measure_idx, int16_t _prop_val, bool brackets, bool measure) {
    const char *fmt =
            brackets && measure ? "[%i]%s" : (brackets & !measure ? "[%i]" : (!brackets && measure ? "%i%s"
                                                                                                   : "%i"));
    sprintf(BUFF, fmt, _prop_val, MEASURES[measure_idx]);
    oled.outputTextXY(DISPLAY_BASE + 2, 64, BUFF, true, false);
}


#ifdef ENC1_PIN
#ifdef ENC2_PIN

ISR(PCINT2_vect) {
    unsigned char result = encoder.process();
    if (result != NOTHING) {
        control_touched = true;
        switch (state) {
            case EDIT_PROP: {
                result == DIR_CW ? DECR(prop_value, prop_min) : INCR(prop_value, prop_max);
                break;
            }
            case BROWSE: {
                result == DIR_CW ? DECR(prop_idx, 0) : INCR(prop_idx, props_idx_max);
                break;
            }
            case STATES: {
                result == DIR_CW ? DECR(state_idx, 0) : INCR(state_idx, state_idx_max);
                break;
            }
        }
    }
}

#endif
#endif

#endif // NO_CONTROLLER
