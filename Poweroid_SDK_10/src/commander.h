#ifndef COMMANDER_H
#define COMMANDER_H


class Commander {
public:

    Commander(Context &_ctx);

    void listen();

    void storeProps();

    void printChangedState(bool prev_state, bool state, uint8_t id);

    void disarmState(uint8_t i, bool disarm);

    bool isConnected();

private:

    bool connected = false;

    Context *ctx;

    const char * printProperty(uint8_t i);

    void printBinProperty(uint8_t i);

    static TimingState connection_check;
};

#endif // COMMANDER_H