//
// Created by SHL on 20.03.2017.
//

#ifndef _SENSORS_H
#define _SENSORS_H

#include "commons.h"
#include "timings.h"
#include <DHT.h>

#define DHTTYPE DHT22


extern const uint8_t IN_PINS[];
extern const uint8_t INA_PINS[];

class Sensors {
public:

    const char *const ORIGIN = "SENSORS";

    Sensors();

    void updateTnH();

    float getTemperature() const;

    float getHumidity() const;

    void process();

    bool checkInstalled(uint8_t pin, bool inst);

    bool checkInstalledWithDelay(uint8_t pin, bool inst, TimingState &hold_on);

    void initSensors(bool _propagate);

    void checkInstalled();

    bool isDhtInstalled();

    bool isSensorOn(uint8_t index);

    int getSensorVal(uint8_t index);

    bool isSensorVal(uint8_t index, uint8_t val);

    const char * printSensor(uint8_t idx);

    uint8_t size();

    const char *printDht();

    void setDht(int8_t _temp, uint8_t _humid);
private:

    float temp;

    float humid;

    DHT dht;

    void printInstalled(uint8_t pin);

    void searchDht();

    int8_t getInt(float f) const;
};


#endif //_SENSORS_H