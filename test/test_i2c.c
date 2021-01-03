#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"
#include "i2c.h"


int main(){
    serial_setup();
    i2c_setup();
    sei();

    serial_open();
    _delay_ms(300);
    serial_write('I');

    uint8_t buf1[10];
    i2cr_status_t status = Running;
    
    for(;;){
        buf1[0] = 0x00;     //Data to be sent (00 apagat)
        i2c_send(0x3F, buf1, 1, &status);
        _delay_ms(2000);

        buf1[0] = 0xFF;     //Data to be sent
        i2c_send(0x3F, buf1, 1, &status);
        _delay_ms(2000);
    }
    return 0;
}