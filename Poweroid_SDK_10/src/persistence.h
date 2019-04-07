#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "properties.h"

#define BASE 0

#define SIGNATURE_SIZE 4
#define HASH_SIZE sizeof(unsigned long)
#define FLAGS_SIZE 1
#define STATES_SIZE 1

#define HASH_OFFSET (BASE + SIGNATURE_SIZE)
#define FLAGS_OFFSET (HASH_OFFSET + HASH_SIZE)
#define STATES_OFFSET (FLAGS_OFFSET + FLAGS_SIZE)
#define PROPS_OFFSET (STATES_OFFSET + STATES_SIZE)
#define PROP_ADDR(x) PROPS_OFFSET + sizeof(long)*(x)

struct Persistence {

    const char *const ORIGIN = "EEPROM";
    uint8_t size;
        char signature[SIGNATURE_SIZE];
    const char *given_sign_chr;
    Property *props;

    Persistence(const char *_sign, Property *props, uint8_t props_size);

    void storeProperties(Property *props);

    void storeState(uint8_t id, bool state);

    void storeProperty(uint8_t i, long val);

    void storeFLags();

    bool loadState(uint8_t id);

    void loadProperties(Property *prop);

    void checkFactoryReset(Property *props);

    void begin();
};


#endif