//
// Created by SHL on 20.03.2017.
//

#ifndef _SENSORS_H
#define _SENSORS_H

#include "commons.h"
#include "timings.h"

#define DHTTYPE DHT22
#define REMOTE_SENSORS_COUNT 3


extern const uint8_t IN_PINS[];
extern const uint8_t INA_PINS[];
extern uint16_t REMOTE_SENSORS[];



class Sensors {
public:

    const char *const ORIGIN = "SENSORS";

    void updateDHT();

    float getTemperature() const;

    float getHumidity() const;

    void process();

    bool checkInstalled(uint8_t pin, bool inst);

    void initSensors(bool _propagate);

    void checkInstalled();

    bool isDhtInstalled();

    bool isSensorOn(uint8_t index);

    int16_t getSensorVal(uint8_t index);

    bool isSensorVal(uint8_t index, uint8_t val);

    uint8_t size();

    const char *printDht();

    void setDHT(int8_t _temp, uint8_t _humid);

    const char *printSensor(uint8_t i);

    const int16_t getNormalizedSensor(uint8_t i, int16_t min, int16_t max, uint16_t raw_min, uint16_t  raw_max);
private:

    float temp;

    float humid;

    void searchDHT();

    int8_t getInt(float f) const;
};


#endif //_SENSORS_H