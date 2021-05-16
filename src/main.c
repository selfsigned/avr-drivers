// TEST MAIN
#include <avr/io.h>
#include <util/delay.h>

#define I2C_MASTER_FREQ 100000UL
#include "i2c_master.h"
#include "i2c_slave.h"

#define I2C_SLAVE_ADDR 0x0B

#include <string.h>

void slave_receive(uint8_t data)
{
    // TODO
}

void slave_send(bool data_ack)
{
    // TEST
    TWDR = (data_ack) ? 42 : 21;
}

int main()
{
#ifdef IS_I2C_MASTER
    uint8_t slave_port_b;
    uint8_t slave_port_c;
    i2c_init();
#endif

#ifdef IS_I2C_SLAVE
    i2c_slave_setcallbacks(slave_receive, slave_send);
    i2c_slave_start(0x0B, false);
    sei();
#endif

    while (1) {
#ifdef IS_I2C_MASTER
        if (i2c_start_receive(I2C_SLAVE_ADDR, I2C_INFINITE_TIMEOUT)
            == I2C_SUCCESS) {
            slave_port_b = i2c_receive(true, 0);
            slave_port_c = i2c_receive(true, 0);
            i2c_stop();

            if (slave_port_b == 42 && slave_port_c == 21) {
                PORTB = 0xff;
                DDRB = 0xff;
            }
        }
        i2c_stop();
#endif

        _delay_ms(100);
    }
}
