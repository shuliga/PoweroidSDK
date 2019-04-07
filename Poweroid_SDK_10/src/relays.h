//
// Created by SHL on 25.03.2017.
//

#ifndef POWEROID_10_RELAYS_H
#define POWEROID_10_RELAYS_H

#include "commons.h"

#ifndef RELAY_ON_LOW
#define RELAY_ON_LOW false
#endif

#define VIRTUAL_RELAYS 2
#define REL_A 0
#define REL_B 1

const uint8_t OUT_PINS[] = {PWR1_PIN, PWR2_PIN};
const uint8_t RELAYS = ARRAY_SIZE(OUT_PINS) + VIRTUAL_RELAYS;

class Relays {

private:

    static bool powered[4];

    static int8_t mappings[VIRTUAL_RELAYS];

    static bool relay_on_low;

public:

    bool mapped = true;

    void power(uint8_t i, bool _power);

    uint8_t size();

    void reset();

    char * printRelay(uint8_t idx);

    unsigned char * relStatus();

    void castRelay(uint8_t idx);

    void castMappedRelays();

    int8_t getMappedFromVirtual(uint8_t i);

    bool isPowered(uint8_t idx);

};


#endif //POWEROID_10_RELAYS_H