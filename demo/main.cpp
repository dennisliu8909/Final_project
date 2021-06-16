#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalInOut pin10(D12);
BufferedSerial uart(D1, D0);
BufferedSerial xbee(D10, D9);

BBCar car(pin5, pin6, servo_ticker);

int main() {
    bool straight = 1;
    bool circle = 1;
    parallax_ping  ping1(pin10);
    uart.set_baud(9600);
    xbee.set_baud(9600);
    char buf[256], outbuf[256];
    FILE *devin = fdopen(&uart, "r");
    FILE *devout = fdopen(&uart, "w");
    while(1) {  
        if (straight) {
            xbee.write("section1:Go!\r\n", 14);
            xbee.write("section1:End!\r\n", 16);
            straight = 0;
        }

        if ((float)ping1 < 20 && circle) {
            xbee.write("section2:Go!\r\n", 14);
            car.stop();
            ThisThread::sleep_for(300ms);
            car.turn2(-100, -0.7);
            ThisThread::sleep_for(600ms);
            car.stop();
            ThisThread::sleep_for(300ms);
            car.goStraight(-100);
            ThisThread::sleep_for(1000ms);
            car.stop();
            ThisThread::sleep_for(300ms);
            car.turn2(-100, 0.7);
            ThisThread::sleep_for(550ms);
            car.stop();
            ThisThread::sleep_for(200ms);
            car.goStraight(-100);
            ThisThread::sleep_for(650ms);
            car.turn2(-100, 0.7);
            ThisThread::sleep_for(550ms);
            car.stop();
            ThisThread::sleep_for(200ms);
            car.goStraight(-100);
            ThisThread::sleep_for(700ms);
            circle = 0;
            xbee.write("section2:End!\r\n", 16);
            xbee.write("section3:Go!\r\n", 14);
            xbee.write("section3:End!\r\n", 16);
            xbee.write("section4:Go!\r\n", 14);
            xbee.write("section4:End!\r\n", 16);
        }
        memset(buf, 0, 256);
        for( int i = 0; ; i++ ) {
            char recv = fgetc(devin);
            buf[i] = fputc(recv, devout);
            if(recv == '\n') {
                printf("\r\n");
                break;
            }
        }
        printf("%s\n",buf);
        RPC::call(buf, outbuf);
    }
}