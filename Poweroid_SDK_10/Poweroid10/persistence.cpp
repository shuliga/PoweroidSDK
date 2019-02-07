
#include <EEPROM.h>
#include "commons.h"
#include "persistence.h"

// Console output codes
// 100 - Signature
// 101 - Hash
// 200 - Factory reset
// 220 - Properties stored in EEPROM.
// 210 - Properties loaded from EEPROM
// 301 - Hash differs from factory.
// 501 - Not a native EEPROM. Overwriting signature.

unsigned long hashProp(Property *props, int size) {
    unsigned long result = 19;
    for (uint8_t i = 0; i < size; i++) {
        result += hash((byte *) props[i].runtime, size * sizeof(long));
    }
    return result;
}

Persistence::Persistence(const char *_sign, Property *_props_runtime, uint8_t props_size) : given_sign_chr(_sign), props(_props_runtime), size(props_size) {}

void Persistence::begin() {
    checkFactoryReset(props);
    EEPROM.get(BASE, signature);
    signature[SIGNATURE_SIZE - 1] = '\0';
#ifdef DEBUG
    writeLog('I', ORIGIN, 100, isprint(signature[0]) ? signature : "___");
#endif
    unsigned long eeprom_hash;
    EEPROM.get(HASH_OFFSET, eeprom_hash);
#ifdef DEBUG
    writeLog('I', ORIGIN, 101, eeprom_hash);
#endif
    if (strcmp(given_sign_chr, signature) != 0)
    {
        strcpy(signature, given_sign_chr);
#ifdef DEBUG
        writeLog('W', ORIGIN, 501, signature);
#endif
        EEPROM.put(BASE, signature);
        storeProperties(props);
    } else
        if (eeprom_hash != hashProp(props, size))
    {
#ifdef DEBUG
        writeLog('W', ORIGIN, 301);
#endif
        loadProperties(props);
    }
    PWR_FLAGS = EEPROM.read(FLAGS_OFFSET);
}

void Persistence::storeProperties(Property *props) {
    for (uint8_t i = 0; i < size; i++) {
        storeProperty(i, props[i].runtime);
    }
    unsigned long hash = hashProp(props, size);
#ifdef DEBUG
    writeLog('I', ORIGIN, 220, size);
    writeLog('I', ORIGIN, 101, hash);
#endif
    EEPROM.put(HASH_OFFSET, hash);
}

void Persistence::storeProperty(uint8_t i, long val) {
    EEPROM.put(PROP_ADDR(i), val);
}

void Persistence::loadProperties(Property *props) {
    for (uint8_t i = 0; i < size; i++) {
        EEPROM.get(PROP_ADDR(i), props[i].runtime);
    }
#ifdef DEBUG
    writeLog('I', ORIGIN, 210, size);
#endif
}

void Persistence::checkFactoryReset(Property *props) {
#ifdef FACTORY_RESET_PIN
    pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);
    if (digitalRead(FACTORY_RESET_PIN) == LOW) {
#ifdef DEBUG
        writeLog('I', ORIGIN, 200);
#endif
        storeProperties(props);
        EEPROM.write(STATES_OFFSET, 0); // Clear state DISARM flags
        EEPROM.write(FLAGS_OFFSET, 0); // Clear PWR_FLAGS
    }
#endif
}

bool Persistence::loadState(uint8_t id) {
    return (bool) ((EEPROM.read(STATES_OFFSET) & (1 << id)) >> id);
}

void Persistence::storeState(uint8_t id, bool state) {
    EEPROM.write(STATES_OFFSET,
                 static_cast<uint8_t>(state ? EEPROM.read(STATES_OFFSET) | (1 << id) : EEPROM.read(STATES_OFFSET) & ~(1 << id)));
}

void Persistence::storeFLags() {
    EEPROM.write(FLAGS_OFFSET, PWR_FLAGS);
}
