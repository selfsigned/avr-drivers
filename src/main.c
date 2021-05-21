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

#include <avr/io.h>
#include <util/delay.h>

#include "i2c_master.h"
#include "i2c_slave.h"

#define I2C_SLAVE_ADDR 0x0B
#define I2C_MASTER_FREQ 350000UL

#include <string.h>

void slave_receive(uint8_t data, bool transmission_start)
{
    static char spi_command_buf[80] = { 0 };
    static int8_t i = 0;

    if (transmission_start)
        i = 0;
    spi_command_buf[i++] = data;

    // dbg
    if (strstr(spi_command_buf, "UwU")) {
        DDRC = (1 << PIN0);
        PORTC = (1 << PIN0);
    }
}

void slave_send(bool transmission_start)
{
    char str[]
        = "What the fuck did you just fucking say about me, you little bitch? "
          "I’ll have you know I graduated top of my class in the Navy Seals";
    static uint8_t i = 0;

    if (transmission_start)
        i = 0;
    TWDR = str[i++];
}

int main()
{
#ifdef IS_I2C_MASTER
    i2c_init(I2C_MASTER_FREQ);
#endif

#ifdef IS_I2C_SLAVE
    i2c_slave_setcallbacks(slave_receive, slave_send);
    i2c_slave_start(I2C_SLAVE_ADDR, false);
    sei();
#endif

    while (1) {
#ifdef IS_I2C_MASTER
        if (i2c_start_receive(I2C_SLAVE_ADDR, I2C_INFINITE_TIMEOUT)
            == I2C_SUCCESS) {
            while (i2c_receive(I2C_ACK, I2C_INFINITE_TIMEOUT) > 0)
                ;
            // read with NACK to end the transmision
            i2c_receive(I2C_NACK, I2C_INFINITE_TIMEOUT);
        }
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
        i2c_stop();
        _delay_ms(2);

        char str[] = "UwU";
        int8_t len = strlen(str);

        if (i2c_start_transmit(I2C_SLAVE_ADDR, I2C_INFINITE_TIMEOUT)
            == I2C_SUCCESS) {
            for (uint8_t i = 0; i < len; i++)
                i2c_transmit(str[i], I2C_INFINITE_TIMEOUT);
            i2c_stop();
        }
        i2c_stop();
        _delay_ms(2);
#endif
    }
}
