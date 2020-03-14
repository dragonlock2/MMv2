#include "mbed.h"

// Custom classes
#include "VL53.h"
#include "QuadEnc.h"
#include "DRV88.h"
#include "IR.h"
#include "Motor.h"
#include "Karel.h"

// Debug
DigitalOut red(PC_15, 1); // note inverted
DigitalOut green(PC_14, 1);
DigitalOut blue(PC_13, 1);
Serial pc(PA_9, PA_10);

// RTOS
Timer tim;

// Sensors
I2C i2c(PB_11, PB_10);
VL53 left(&i2c, &tim, PB_1, PB_2); // XSHUT, GPIO
VL53 center(&i2c, &tim, PA_4, PA_5);
VL53 right(&i2c, &tim, PA_2, PA_3);

IR left_ir(PA_7);
IR right_ir(PA_0);

QuadEnc leftenc(PB_7, PB_6, &tim);
QuadEnc rightenc(PB_9, PB_8, &tim, true);

// Outputs
DRV88 leftdrv(PB_5, PB_4, true);
DRV88 rightdrv(PA_15, PB_3);

Motor motor(&leftenc, &rightenc, &leftdrv, &rightdrv);

int main()
{
    pc.baud(115200);

    i2c.frequency(125000);

    tim.start();

    // Sensor Setup
    VL53::init(&red);
    left.setup(0x01);
    center.setup(0x02);
    right.setup(0x03);

    IR::init(5); // ~200Hz polling

    rightenc.setup();
    leftenc.setup();
    ThisThread::sleep_for(10);

    // Output Setup
    motor.init(5); // ~200Hz PID
    motor.setLinearPID(0.04, 0.01, 0.001);
    motor.setLinearIntegralConstraint(100);
    motor.setAngularPID(0.5, 0.1, 0.01);
    motor.setAngularIntegralConstraint(5);

    Karel karel(&left, &center, &right, &left_ir, &right_ir, &motor, 980, 20);

    // Test
    // motor.move(980, 0);

    char data[3];
    data[0] = 0x6A; // USER_CTRL
    data[1] = 0x00; // disable master i2c
    i2c.write(0x68 << 1, data, 2);

    data[0] = 0x37; // INT_PIN_CFG
    data[1] = 0x02; // enable bypass to access ak8963
    i2c.write(0x68 << 1, data, 2);

    data[0] = 0x0A; // CTRL1 register
    data[1] = 0b00010110; // 100Hz, 16-bit output
    i2c.write(0x0C << 1, data, 2);

    int16_t mags[4]; // x,y,z, trash (for st2)

    while (true) {
        // pc.printf("%d %f %d %f\n", leftenc.count, leftenc.velocity(), rightenc.count, rightenc.velocity());
        // pc.printf("%d %d %d\n", left.dist, center.dist, right.dist);
        // pc.printf("%d %d\n", left_ir.reading, right_ir.reading);

        // pc.printf("%d %f %d %f %d %d %d %d %d\n", leftenc.count, leftenc.velocity(), rightenc.count, rightenc.velocity(), 
        //     left.dist, center.dist, right.dist, 
        //     left_ir.reading, right_ir.reading);

        // pc.printf("%f %f\n", motor.getLinearVelocity(), motor.getAngularVelocity() * 100);

        // karel.wallFollowLeft();

        data[0] = 0x02;
        i2c.write(0x0C << 1, data, 1);
        i2c.read(0x0C << 1, data, 1);
        if (data[0]) {
            data[0] = 0x03;
            i2c.write(0x0C << 1, data, 1);
            i2c.read(0x0C << 1, (char *) mags, 7);
        }

        pc.printf("%d, %d, %d\n", mags[0], mags[1], mags[2]);

        ThisThread::sleep_for(20);
    }
}