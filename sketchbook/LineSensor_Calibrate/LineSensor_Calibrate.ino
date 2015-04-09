#include <Arduino.h>
#include <QTRSensors.h>

#include <line_sensor.h>

LineSensors bar;

void setup() {
    Serial.begin(115200);
    Serial.print("opening sensor bar..");
    // use the default values as assigned at line_sesnor::init()
    bar.init(10000);
    Serial.println("  OK");
}

void loop() {
    bar.poll_sensors();
    Serial.print(bar.read_line(), DEC);
    Serial.print("\t");
    for (int sth = 0; sth < 8; sth++) {
        Serial.print(sth, DEC);
        Serial.print(": ");
        Serial.print(bar.value(sth), DEC);
        Serial.print("\t");
    }   Serial.println();
    delay(250);
}
