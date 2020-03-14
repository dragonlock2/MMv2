#include "QuadEnc.h"

QuadEnc::QuadEnc(PinName a, PinName b, Timer *tim, bool reverse, us_timestamp_t timeout_us) :
    a(reverse ? b : a),
    b(reverse ? a : b),
    tim(tim),
    timeout_us(timeout_us)
{}

void QuadEnc::setup() {
    // really easy to change to other encodings, just comment some of these lines out
    a.rise(callback(this, &QuadEnc::aRise));
    // a.fall(callback(this, &QuadEnc::aFall));
    // b.rise(callback(this, &QuadEnc::bRise));
    // b.fall(callback(this, &QuadEnc::bFall));

    count = 0;
    prevTime = tim->read_high_resolution_us();
}

float QuadEnc::velocity() {
    if (currTime < tim->read_high_resolution_us() - timeout_us) {
        return 0;
    } else {
        if (reverse) {
            return -UNITS_PER_SECOND / (currTime - prevTime);
        } else {
            return UNITS_PER_SECOND / (currTime - prevTime);
        }
    }
}

void QuadEnc::aRise() {
    if (b.read()) {
        count--;
        reverse = true;
    } else {
        count++;
        reverse = false;
    }
    prevTime = currTime;
    currTime = tim->read_high_resolution_us();
}

void QuadEnc::aFall() {
    if (b.read()) {
        count++;
        reverse = false;
    } else {
        count--;
        reverse = true;
    }
    prevTime = currTime;
    currTime = tim->read_high_resolution_us();
}

void QuadEnc::bRise() {
    if (a.read()) {
        count++;
        reverse = false;
    } else {
        count--;
        reverse = true;
    }
    prevTime = currTime;
    currTime = tim->read_high_resolution_us();
}

void QuadEnc::bFall() {
    if (a.read()) {
        count--;
        reverse = true;
    } else {
        count++;
        reverse = false;
    }
    prevTime = currTime;
    currTime = tim->read_high_resolution_us();
}