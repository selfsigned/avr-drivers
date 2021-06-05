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

/// @file i2c_master.c
/// @author selfsigned
/// @date 2021
/// @brief AVR I²C master driver implementation.

#include "i2c_master.h"
#include <avr/io.h>
#include <util/twi.h>

// LOCAL

typedef enum i2c_direction_e { I2C_TRANSMIT, I2C_RECEIVE } i2c_direction_t;

static uint32_t (*I2C_TIMEOUT_TIMER)(); // timer reader function

static bool wait_on_twi(uint16_t timeout)
{
    if (timeout == I2C_INFINITE_TIMEOUT) {
        while (!(TWCR & (1 << TWINT))) { }
    } else {
        uint16_t timer = I2C_TIMEOUT_TIMER();
        while (!(TWCR & (1 << TWINT))) {
            if (I2C_TIMEOUT_TIMER() - timer >= timeout)
                return I2C_TIMEOUT;
        }
    }

    return I2C_SUCCESS;
}

static inline i2c_status_t i2c_start(
    uint8_t slave_addr, bool direction, uint16_t timeout)
{
    // reset
    TWCR = 0;
    // TWINT == clear interrupts | TWSTA == master | TWEN == enable i2c module
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    // wait on TWINT
    if (wait_on_twi(timeout) != I2C_SUCCESS)
        return I2C_TIMEOUT;
    // check TWI value
    if (TW_STATUS != TW_START)
        return I2C_ERROR;

    // transmit slave addr
    TWDR = (slave_addr << 1) | direction;
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (wait_on_twi(timeout) != I2C_SUCCESS)
        return I2C_TIMEOUT;
    // check if r|w mode acknowledged
    if (TW_STATUS != TW_MR_SLA_ACK && TW_STATUS != TW_MT_SLA_ACK)
        return I2C_ERROR;
    return I2C_SUCCESS;
}

// PUBLIC

void i2c_set_timer_callback(uint32_t (*timer)()) { I2C_TIMEOUT_TIMER = timer; }

void i2c_init(uint32_t i2c_freq)
{
    // set i2c rate
    TWBR = (((F_CPU / i2c_freq) - 16) / 2);

    // pullup
#if defined __AVR_ATmega32U4__ || defined __AVR_ATmega16U4__
    PORTD |= 1 << PORTD0;
    PORTD |= 1 << PORTD1;
#elif defined __AVR_ATmega48P__ || defined __AVR_ATmega48A__                   \
    || defined __AVR_ATmega48PA__ || defined __AVR_ATmega88A__                 \
    || defined __AVR_ATmega88PA__ || defined __AVR_ATmega168A__                \
    || defined __AVR_ATmega168PA__ || defined __AVR_ATmega328__                \
    || defined __AVR_ATmega328P__ || defined __AVR_ATmega328PU__
    PORTC |= 1 << PORTC4;
    PORTC |= 1 << PORTC5;
#endif
}

i2c_status_t i2c_start_transmit(uint8_t slave_address, uint16_t timeout)
{
    return i2c_start(slave_address, I2C_TRANSMIT, timeout);
}

i2c_status_t i2c_start_receive(uint8_t slave_address, uint16_t timeout)
{
    return i2c_start(slave_address, I2C_RECEIVE, timeout);
}

i2c_status_t i2c_transmit(uint8_t data, uint16_t timeout)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    if (wait_on_twi(timeout) != I2C_SUCCESS)
        return I2C_TIMEOUT;
    if (TW_STATUS != TW_MT_DATA_ACK)
        return I2C_ERROR;
    return I2C_SUCCESS;
}

i2c_data_return_t i2c_receive(bool ack, uint16_t timeout)
{
    TWCR = (ack) ? (1 << TWEA) | (1 << TWINT) | (1 << TWEN)
                 : (1 << TWINT) | (1 << TWEN);
    if (wait_on_twi(timeout) != I2C_SUCCESS)
        return I2C_TIMEOUT;

    return TWDR;
}

void i2c_stop(void) { TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); }