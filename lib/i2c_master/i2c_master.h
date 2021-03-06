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

/// @file i2c_master.h
/// @author selfsigned
/// @date 2021
/// @brief AVR I²C master driver.

#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include <inttypes.h>
#include <stdbool.h>

/// the value that triggers an infinite wait for ACK
#define I2C_INFINITE_TIMEOUT 0

typedef int16_t i2c_status_t;
typedef int16_t i2c_data_return_t;
#define I2C_SUCCESS 0
#define I2C_ERROR -1
#define I2C_TIMEOUT -2
#define I2C_NACK_RECV -3

#define I2C_ACK true
#define I2C_NACK false

/// Set the timer function
/// @param timer called when a timestamp is requested or timeout is superior to
/// I2C_INFINITE_TIMEOUT (0)
void i2c_set_timer_callback(uint32_t (*timer)());

/// Initialize the I²C interface in Master mode.
/// @param i2c_freq The transmission frequency, 100000UL is a good value to
/// start with
void i2c_init(uint32_t i2c_freq);

/// Starts an I²C transaction as master, sending data
/// @param slave_address the address of the slave device
/// @param timeout delay after which the transmission fails
i2c_status_t i2c_start_transmit(uint8_t slave_address, uint16_t timeout);

/// Starts an I²C transaction as master, requesting data
/// @param slave_address the address of the slave device
/// @param timeout delay after which the transmission fails
i2c_status_t i2c_start_receive(uint8_t slave_address, uint16_t timeout);

/// Transmit a byte during an I²C transaction, Master Transmitter mode.
/// @param data byte to send.
/// @param timeout delay after which the transmission fails
i2c_status_t i2c_transmit(uint8_t data, uint16_t timeout);

/// Read a single byte during an I²C transaction, Master receiver mode. Make
/// sure to send a NACK read request at the end of the transmission!
/// @param ack I2C_ACK (true) to ACKnowledge the transmission, I2C_NACK (false)
/// not to.
/// @return data read from the bus.
i2c_data_return_t i2c_receive(bool ack, uint16_t timeout);

/// Stops the I²C transaction
void i2c_stop(void);

#endif