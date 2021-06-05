/* Copyright (C) 2021 selfsigned

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/// @file master.c
/// @author selfsigned
/// @date 2021
/// @brief Custom drivers implementation examples

// used for timers
#include <Arduino.h>

#include <avr/io.h>
#include <util/delay.h>

extern "C" {
#include "i2c_master.h"
#include "i2c_slave.h"
}

#define I2C_SLAVE_ADDR 0x0B
#define I2C_MASTER_FREQ 200000UL

#define I2C_TIMEOUT_VAL 100

#include <string.h>

bool slave_receive(uint8_t data, bool transmission_start)
{
    static char buf[80] = { 0 };
    static int8_t i = 0;

    if (!data)
        return false;
    if (transmission_start)
        i = 0;
    buf[i++] = data;

    // dbg
    if (strstr(buf, "UwU")) {
        DDRC = (1 << PIN0);
        PORTC = (1 << PIN0);
    }
    return true;
}

void slave_send(bool transmission_start)
{
    char str[] = "OwO";
    static uint8_t i = 0;

    if (transmission_start) {
        i = 0;
    }
    TWDR = str[i++];
    return;
}

void setup()
{
#ifdef IS_I2C_MASTER
    i2c_set_timer_callback(millis);
    i2c_init(I2C_MASTER_FREQ);
#endif

#ifdef IS_I2C_SLAVE
    i2c_slave_setcallbacks(slave_receive, slave_send);
    i2c_slave_start(I2C_SLAVE_ADDR, false);
    sei();
#endif

    // Debug
    Serial.begin(6400);
}

void loop()
{
#ifdef IS_I2C_MASTER
    switch (i2c_start_receive(I2C_SLAVE_ADDR, I2C_TIMEOUT_VAL)) {

    case I2C_SUCCESS:
        i2c_data_return_t ret;

        while ((ret = i2c_receive(I2C_ACK, I2C_TIMEOUT_VAL)) > 0)
            Serial.write(ret);
        if (ret == I2C_SUCCESS) {
            Serial.write('\n');
            i2c_receive(I2C_NACK, I2C_TIMEOUT_VAL);
        } else
            Serial.write("Receiver error or timeout while reading data\n");
        break;

    case I2C_TIMEOUT:
        Serial.write("Receiver Timeout\n");
        break;

    case I2C_ERROR:
        Serial.write("Receiver Error\n");
        break;

    default:
        break;
    }
    i2c_stop();
    _delay_ms(2);

    char str[20] = { 0 };
    strcpy(str, "UwU");
    int8_t len = strlen(str);
    i2c_status_t stat;

    if ((stat = i2c_start_transmit(I2C_SLAVE_ADDR, I2C_TIMEOUT_VAL))
        == I2C_SUCCESS) {
        uint8_t i = 0;
        for (; i < len && i2c_transmit(str[i], I2C_TIMEOUT_VAL) == I2C_SUCCESS;
             i++)
            ;
        if (i != len)
            Serial.println("Transmitter error\n");
    }
    i2c_stop();
    _delay_ms(2);
#endif
}