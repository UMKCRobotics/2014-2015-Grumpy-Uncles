#include <SPI.h>
#include <Segment.h>
#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>
#include <Cardinal.h>
#include <QTRSensors.h>
#include <line_sensor.h>
#include <motorCommander.h>
#include <arduinoConfig.h>

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

QTRSensorsRC bar;
unsigned int values[NUM_SENSORS];
uint8_t pins[NUM_SENSORS] = { 14, 15, 16, 17, 18, 19, 34, 35 };

void setup() {
    // the SPI device must come up before the LEDs are used.
    SPI.begin();
    SPI.setClockDivider(128);
    SPI.setBitOrder(MSBFIRST);
    
    Serial.begin(115200);
    pinMode(A3, INPUT);
    mc.init();
    bar.init(pins, NUM_SENSORS, TIMEOUT, QTR_NO_EMITTER_PIN);

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
                mc.MOVE_FORWARD(false);
                break;
            case 's':
                mc.MOVE_BACKWARD();
                break;
            case 'a':
                mc.TURN_LEFT();
                break;
            case 'd':
                mc.TURN_RIGHT();
                break;
            case 'x':
                mc.STOP();
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
    
    bar.read(values);
    Serial.print("  [ ");
    for (int i = 0; i < 8; i++) {
        Serial.print(values[i], DEC);
        Serial.print(" ");
    }

    drift = bar.readLine(values) - 3500;
    Serial.print("] ");
    Serial.print(drift, DEC);
    
    Serial.println();
    delay(200);
}
