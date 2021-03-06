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

static bool (*I2C_SLAVE_RECEIVE_FUNC)(uint8_t, bool); // recv interrupt callback
static void (*I2C_SLAVE_SEND_FUNC)(bool); // send interrupt callback

void i2c_slave_setcallbacks(bool (*receive)(uint8_t, bool), void (*send)(bool))
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
    bool ack = true;

    switch (TW_STATUS) {
    // receive data
    case TW_SR_SLA_ACK:
        ack = I2C_SLAVE_RECEIVE_FUNC(TWDR, I2C_SLAVE_TR_START);
        break;
    case TW_SR_DATA_ACK:
        ack = I2C_SLAVE_RECEIVE_FUNC(TWDR, I2C_SLAVE_TR_CONTINUES);
        break;

    // send data
    case TW_ST_SLA_ACK:
        I2C_SLAVE_SEND_FUNC(I2C_SLAVE_TR_START);
        break;
    case TW_ST_DATA_ACK:
        I2C_SLAVE_SEND_FUNC(I2C_SLAVE_TR_CONTINUES);
        break;

    // handle errors
    case TW_BUS_ERROR:
        TWCR = (1 << TWSTO) | (1 << TWINT);
        break;

    default:
        break;
    }

    // execute, ACKnowledge
    TWCR |= (1 << TWINT) | (1 << TWEA);
    // TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE) | (1 << TWINT)
    // | (ack << TWEA);
}