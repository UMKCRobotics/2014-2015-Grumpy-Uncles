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

void setup() {
    SPI.begin();
    SPI.setClockDivider(128);
    SPI.setBitOrder(MSBFIRST);
    
    Serial.begin(115200);
   
    pinMode(A3, INPUT);
    
    mc.init(A3);
}

short dcatch = 0;
short throttle = 0;
short cell = 0;
short light = 0;

char buffer[5];

void loop() {
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
