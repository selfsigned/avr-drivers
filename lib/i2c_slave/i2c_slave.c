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
#include "i2c_slave.h"

static void (*I2C_SLAVE_RECEIVE_FUNC)(uint8_t); // recv interrupt callback
static void (*I2C_SLAVE_SEND_FUNC)(bool); // send interrupt callback

void i2c_slave_setcallbacks(void (*receive)(uint8_t), void (*send)(bool))
{
    I2C_SLAVE_RECEIVE_FUNC = receive;
    I2C_SLAVE_SEND_FUNC = send;
}

void i2c_slave_start(uint8_t address, bool respond_to_gce)
{
    TWAR = (respond_to_gce) ? (address << 1) | (1 << TWGCE) : (address << 1);
    TWCR = (1 << TWEA) | // addr ack
        (1 << TWEN) | // enable device
        (1 << TWIE) | // enable  i²c interrupts
        (1 << TWINT); // clear the interrupt flag
}

void i2c_slave_stop()
{
    TWAR = 0;
    TWCR = 0;
}

ISR(TWI_vect)
{
    switch (TW_STATUS) {

    // receive data
    case TW_SR_DATA_ACK:
        I2C_SLAVE_RECEIVE_FUNC(TWDR);
        break;

    // send data
    case TW_ST_SLA_ACK:
        I2C_SLAVE_SEND_FUNC(true);
        break;
    case TW_ST_DATA_ACK:
        I2C_SLAVE_SEND_FUNC(false);
        break;

    // handle errors
    case TW_BUS_ERROR:
        TWCR |= (1 << TWSTO) | (1 << TWINT);
        break;

    default:
        break;
    }

    // execute, ACKnowledge
    TWCR |= (1 << TWINT) | (1 << TWEA);
}