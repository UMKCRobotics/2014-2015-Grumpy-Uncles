#define GDEBUG

#include <SPI.h>
#include <Segment.h>
#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>
#include <Cardinal.h>
#include <QTRSensors.h>
#include <line_sensor.h>
#include <motorCommander.h>
#include <arduinoConfig.h>
#include <Arduino.h>

//For our robot we will be using the i.Mx6's hardware SPI 2
//  which is connected to pins
//  51 - MOSI AKA data
//  52 - CLK
//  53 - SS2 AKA enable
LED marquee;
motorCommander mc;
char synack = 0x00;
Configurator configuration;

#define NUM_SENSORS 8
#define TIMEOUT     2500
#define EMITTER_PIN QTR_NO_EMITTER_PIN

// rear bar (backwards)
//QTRSensorsRC Rbar;
LineSensors Rbar;
const uint8_t Rpins[NUM_SENSORS] = { 14, 15, 16, 17, 18, 19, 34, 35 };
unsigned int Rvalues[NUM_SENSORS];

// front bar
//QTRSensorsRC Fbar;
LineSensors Fbar;
const uint8_t Fpins[6] = {43, 45, 47, 42, 44, 46};
unsigned int Fvalues[6];

LineSensors Bbar;
const uint8_t Bpins[2] = { 29, 28 };
unsigned int Bvalues[2];

void setup() {
    // the SPI device must come up before the LEDs are used.
    SPI.begin();
    SPI.setClockDivider(128);
    SPI.setBitOrder(MSBFIRST);
    
    Serial.begin(115200);
    pinMode(A3, INPUT);
    mc.init(1200, 1200, 2500);
    Fbar.init(Fpins, 6, 1200);
    Rbar.init(Rpins, 8, 1200);
    Bbar.init(Bpins, 2, 2500);

    // indicate that we're up and waiting on sync.
    marquee.light(LED::YELLOW);
    configuration.initialize();
    
    Serial.println("waiting on sync -- press 's'");
    // this sync will eventually change to use config's
    //      OP_SYN and OP_ACK
    do {
        synack = Serial.read();
    } while (synack != 's');
    Serial.write('a');
    
}

short dcatch = 0;
short throttle = 0;
short cell = 0;
short light = 0;
short drift = 0;

char buffer[5];
char cmd_mask = 0xF0;

void loop() {
    // look to see if there's a command waiting on the serial line.
    if (Serial.available() > 0) {
        synack = Serial.read();
        
        // what command was it?
        switch (synack) {
            case 'w':
                // move_forward accepts a boolean to control line_following
                //    true: use drift adjustments
                //    false: ignore the line_sensor and adjustments.
                mc.MOVE_FORWARD();
                break;
            case 'a':
                mc.TURN_LEFT();
                break;
            case 'd':
                mc.TURN_RIGHT();
                break;
            default:
                break;
        }
    }
    
    dcatch = analogRead(A3);
    Serial.print(dcatch, DEC);
    Serial.print("  ");

    throttle = map(dcatch, 0, 1023, 255, 0);
    Serial.print(throttle, DEC);
    Serial.print("  ");

    Serial.print(mc.get_odo_left(), DEC);
    Serial.print("  ");
    Serial.print(mc.get_odo_right(), DEC);
    Serial.print("  ");
    
    light = map(dcatch, 0, 1023, 0, 15);
    marquee.light(light);
    Serial.print(light, HEX);
    Serial.print("  ");
    
    cell = map(dcatch, 0, 1023, 0, 99);
    marquee.display(cell);
    Serial.print(cell, DEC);
    Serial.print("  ");

    configuration.setRound();
    configuration.setPart();
    Serial.print(configuration.getRound(), DEC);
    Serial.print("  ");
    Serial.print(configuration.getPart(), DEC);
    Serial.print("  ");
    
    marquee.inspect(buffer);
    Serial.print(dcatch, DEC);
    Serial.print("  0x");
    for (int i = 0; i < 5; i++) {
        Serial.print(buffer[i], HEX);
    }
    
    Serial.println();
//    Fbar.read(Fvalues);
    Fbar.poll_sensors();
    Serial.print("F  [ ");
    for (int i = 0; i < 6; i++) {
        Serial.print(Fbar.white(i), DEC);
        Serial.print(" ");
    }   Serial.print("] [");
    for (int i = 0; i < 6; i++) {
        Serial.print(Fbar.value(i), DEC);
        Serial.print(" ");
    }
    Serial.print("] ");
    drift = Fbar.read_line();
    Serial.println(drift, DEC);
    
//    Rbar.read(Rvalues);
    Rbar.poll_sensors();
    Serial.print("R  [ ");
    for (int i = 0; i < 8; i++) {
        Serial.print(Rbar.white(i), DEC);
        Serial.print(" ");
    }   Serial.print("] [");
    for (int i = 0; i < 8; i++) {
        Serial.print(Rbar.value(i), DEC);
        Serial.print(" ");
    }
    Serial.print("] ");
    drift = Rbar.read_line();
    Serial.println(drift, DEC);
    
    Bbar.poll_sensors();
    Serial.print("B  [ ");
    for (int i = 0; i < 2; i++) {
        Serial.print(Bbar.white(i), DEC);
        Serial.print(" ");
    }   Serial.print("] [");
    for (int i = 0; i < 2; i++) {
        Serial.print(Bbar.value(i), DEC);
        Serial.print(" ");
    }
    Serial.print("] ");
    drift = Bbar.read_line();
    Serial.println(drift, DEC);
    
    if (configuration.lobatt()) {
        marquee.display(marquee.LO);
        delay(1000);
    }
    Serial.println();
    delay(200);
    mc.poke();
    mc.dump();
    Serial.println();
}
