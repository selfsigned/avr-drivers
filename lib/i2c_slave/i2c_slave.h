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

/// @file i2c_slave.h
/// @author selfsigned
/// @date 2021
/// @brief AVR I²C slave driver.

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/twi.h>

#include <inttypes.h>
#include <stdbool.h>

// The transmission just started
#define I2C_SLAVE_TR_START true
// The transmission is continuing
#define I2C_SLAVE_TR_CONTINUES false

/// Set the functions called when the slave is addressed
/// @param receive called when the master sent data, returns true if ack
/// @param send called when the master requests data, put your data in TWDR (1
/// byte), bool true if called at the start of a transmission
void i2c_slave_setcallbacks(bool (*receive)(uint8_t, bool), void (*send)(bool));

/// Initialize the I²C slave interface. don't forget to call sei()
/// @param address address the device will respond to.
/// @param respond_to_gce true to respond to the general call address
void i2c_slave_start(uint8_t address, bool respond_to_gce);

/// Disables the I²C slave interface
void i2c_slave_stop();

#endif