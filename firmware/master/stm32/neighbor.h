/*
 * Thundercracker Firmware -- Confidential, not for redistribution.
 * Copyright <c> 2012 Sifteo, Inc. All rights reserved.
 */

#ifndef _NEIGHBOR_H
#define _NEIGHBOR_H

#include <stdint.h>
#include "gpio.h"
#include "hwtimer.h"

class Neighbor {
public:
    Neighbor(GPIOPin _nbr_in1,
              GPIOPin _nbr_in2,
              GPIOPin _nbr_in3,
              GPIOPin _nbr_in4,
              GPIOPin _nbr_out1,
              GPIOPin _nbr_out2,
              GPIOPin _nbr_out3,
              GPIOPin _nbr_out4,
              HwTimer _output_timer,
              HwTimer _input_timer) :
                in1(_nbr_in1),
                in2(_nbr_in2),
                in3(_nbr_in3),
                in4(_nbr_in4),
                out1(_nbr_out1),
                out2(_nbr_out2),
                out3(_nbr_out3),
                out4(_nbr_out4),
                txPeriodTimer(_output_timer),
                rxPeriodTimer(_input_timer)
    {}

    void init();

    void beginReceiving();
    void beginTransmit(uint16_t data);
    bool isTransmitting() {
        return txData > 0;
    }

    void transmitNextBit();
    void onRxPulse(uint8_t side);
    void rxPeriodIsr();
    uint16_t getLastRxData();

private:
    // pwm duty specifying the duration of a tx pulse
    static const unsigned TX_ACTIVE_PULSE_DUTY = 25;
    // number of bits to wait for during an rx sequence
    static const unsigned NUM_RX_BITS = 16;

    void setDuty(uint16_t duty);
    void enablePwm();
    void disablePwm();

    uint16_t txData;    // data in the process of being transmitted. if 0, we're done.
    uint8_t input_bit_counter;

    GPIOPin in1;
    GPIOPin in2;
    GPIOPin in3;
    GPIOPin in4;

    GPIOPin out1;
    GPIOPin out2;
    GPIOPin out3;
    GPIOPin out4;

    HwTimer txPeriodTimer;
    HwTimer rxPeriodTimer;

    int8_t receiving_side;
    uint16_t received_data_buffer;
    uint16_t lastRxData;

    enum RxState {
        Squelch,
        WaitingForNextBit,
        WaitingForStart
    };

    RxState rxState;
};

#endif