#ifndef PROPS_H
#define PROPS_H

#include <../Poweroid_SDK_10/src/properties.h>
#include <../Poweroid_SDK_10/src/commons.h>
#include <../Poweroid_SDK_10/src/timings.h>

#define GET_PROP_NORM(i) PROPS.FACTORY[(i)].runtime / PROPS.FACTORY[(i)].scale

static struct Properties {

    int8_t DEFAULT_PROPERTY = 1;

    Property FACTORY[5] = {

            //val      minV     maxV       scale
            {200,      50L,     300L,     1L},
            {250,      100L,    400L,     1L},
            {100L,     50L,     150L,     1L},
            {15L,      5L,      30L,      60000L},
            {2L,       1L,      8L,      3600000L}
    };

    uint8_t props_size = ARRAY_SIZE(FACTORY);

// Property strings should be stored in FlashRAM
// to save program RAM space
// Using F() macros here
    Properties() {
        factoryReset();
        FACTORY[0].desc = F("Water level min (cm)");
        FACTORY[0].measure = CM;

        FACTORY[1].desc = F("P-IN max (kPa)");
        FACTORY[1].measure = KPA;

        FACTORY[2].desc = F("P-IN min (kPa)");
        FACTORY[2].measure = KPA;

        FACTORY[3].desc = F("Pump timeout (sec)");
        FACTORY[3].measure = SEC;

        FACTORY[4].desc = F("Pump switch (hrs)");
        FACTORY[4].measure = HOUR;

        FACTORY[5].desc = F("Pump alarm (sec)");
        FACTORY[5].measure = SEC;

        FACTORY[6].desc = F("Pump pre-up (sec)");
        FACTORY[6].measure = SEC;
    }

    void factoryReset() {
        for (int i = 0; i < ARRAY_SIZE(FACTORY); i++) {
            FACTORY[i].runtime = FACTORY[i].val;
        }
    }

} PROPS;

#endif
