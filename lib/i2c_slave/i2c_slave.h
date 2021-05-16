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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include <inttypes.h>
#include <stdbool.h>

/// Set the functions called when the slave is addressed
/// @param receive called when the master sent data
/// @param send called when the master requests data, put your data in TWDR (1 byte)
void i2c_slave_setcallbacks(void (*receive)(uint8_t), void (*send)(bool));

/// Initialize the I²C slave interface. don't forget to call sei()
/// @param address address the device will respond to.
/// @param respond_to_gce true to respond to the general call address, false not to.
void i2c_slave_start(uint8_t address, bool respond_to_gce);

/// Disables the I²C slave interface
void i2c_slave_stop();

#endif