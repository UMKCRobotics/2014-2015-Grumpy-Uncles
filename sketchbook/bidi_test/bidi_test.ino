#include <SPI.h>
#include <Segment.h>
#include <motorCommander.h>

//For our robot we will be using the i.Mx6's hardware SPI 2
//  which is connected to pins
//  51 - MOSI AKA data
//  52 - CLK
//  53 - SS2 AKA enable
LED marquee;
motorCommander mc;
char synack = 0x00;

void setup() {
    // the SPI device must come up before the LEDs are used.
    SPI.begin();
    SPI.setClockDivider(128);
    SPI.setBitOrder(MSBFIRST);
    
    Serial.begin(115200);
    pinMode(A3, INPUT);
    mc.init(A3);

    // indicate that we're up and waiting on sync.
    marquee.light(LED::YELLOW);
    
    // this sync will eventually change to use config's
    //      OP_SYN and OP_ACK
    do {
        synack = Serial.read();
    } while (synack != 'o');
    Serial.write('k');
    
}

short dcatch = 0;
short throttle = 0;
short cell = 0;
short light = 0;

char buffer[5];
char cmd_mask = 0xF0;

void loop() {
    // look to see if there's a command waiting on the serial line.
    if (Serial.available() > 0) {
        synack = Serial.read();
        
        // what command was it?
        switch(synack & cmd_mask) {
            case 0xE0: // light an LED
                marquee->light(synack & 0x0F);
                break;
        }
    }
    
    dcatch = analogRead(A3);
    Serial.print(dcatch, DEC);
    Serial.print("  ");

    throttle = map(dcatch, 0, 1023, 255, 0);
    mc.MOVE_FORWARD();
    Serial.print(throttle, DEC);
    Serial.print("  ");

    light = map(dcatch, 0, 1023, 0, 15);
    marquee.light(light);
    Serial.print(light, HEX);
    Serial.print("  ");
    
    cell = map(dcatch, 0, 1023, 0, 99);
    marquee.display(cell);
    Serial.print(cell, DEC);
    Serial.print("  ");

    marquee.inspect(buffer);
    Serial.print(dcatch, DEC);
    Serial.print("  ");
    for (int i = 0; i < 5; i++) {
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    delay(200);
}
