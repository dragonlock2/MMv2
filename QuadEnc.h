#ifndef __QUADENC_H__
#define __QUADENC_H__

#include "mbed.h"

#define UNITS_PER_SECOND 1000000.0

class QuadEnc {
    public:
        volatile int32_t count; // pulses
        // will literally take days to overflow with our motors

        QuadEnc(PinName a, PinName b, Timer *tim, bool reverse = false, us_timestamp_t timeout_us = 50000);
        void setup();
        float velocity(); // pulses per second
    private:
        InterruptIn a;
        InterruptIn b;
        Timer *tim;
        const us_timestamp_t timeout_us;
        volatile us_timestamp_t currTime;
        volatile us_timestamp_t prevTime;
        volatile bool reverse;

        void aRise();
        void aFall();
        void bRise();
        void bFall();
};

#endif