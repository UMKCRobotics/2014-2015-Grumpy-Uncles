/* welcome to branch mixer.
 *
 * this is a second (date, 02/09/15) merge of both
 *    arduino and udoo branches of grumpy uncle.
 *
 * the primary branch is 'arduino'.
 *
 * for reference, look at the branch sandbox.
 */

//#define GDEBUG
#undef GDEBUG

#include <SPI.h>
#include <Segment.h>
#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>
#include <Cardinal.h>
#include <QTRSensors.h>
#include <line_sensor.h>
#include <motorCommander.h>
#include <worldSensor.h>
#include <mazeSolver.h>
#include <arduinoConfig.h>

LED marquee;
byte throttle, c_round, c_part;
char synack;
dir::Cardinal nextMove;
short cell;
Configurator configuration;
motorCommander mc;
mazeSolver ms;

void setup() {
    Serial.begin(115200);
    marquee.init();    
    configuration.initialize();
    mc.init(1200, 1200, 2500);
}

const char cmd_led = 0xE0;
boolean synched = false;
boolean reset_held = false;
unsigned long reset_wait = 0;
unsigned long release_time = 0;
const byte gopin = 41;

void loop() {
    // first, check to make sure that we've been synchronized
    //    with the upper half. if not, re-read the configuration
    //    and wait on the sync byte.
    if (synched == false) {
        configuration.setRound();
        configuration.setPart();
        switch(configuration.getRound()){
            case 1: case 2:
                cell = 48;
                break;
            case 3:
                cell = 49;
                break;
        }
        // maybe this will remind someone that we
        //    should start out facing north.
        //
        // you know -- in case they forget that but
        //    remember how to read the coded direction.
        mc.set_direction(dir::NORTH);
        marquee.direction(mc.get_direction());
        marquee.display(marquee.OK);
        marquee.light(LED::YELLOW);
        do {
            synack = Serial.read();
        } while (synack != Configurator::OP_SYN);
        Serial.write(Configurator::OP_ACK);
        configuration.sendConfig(&Serial);

        synched = true;
    } else {
        if (reset_held == false) {
            reset_held = !digitalRead(gopin);
        } else {
            reset_wait =  millis();
            do {
                release_time = millis() - reset_wait;
            } while ((!digitalRead(gopin)) && (release_time < 3000));
            if ((!digitalRead(gopin)) && (release_time >= 3000)) {
                synched = false;
                reset_held = false;
                while(!digitalRead(gopin)) {
                    marquee.light(LED::RED | LED::GREEN | LED::YELLOW);
                }
            } else {
                reset_held = false;
                release_time = 0;
                reset_wait = 0;
            }
        }

        if ((reset_held == false) && (synched == true) && (Serial.available() > 0)) {
            synack = Serial.read();
            if ((synack & cmd_led) == cmd_led) {
                marquee.light(synack & 0x0F);
            } else if (synack == Configurator::OP_MOVE) {
                nextMove = ms.computeNextmove(mc.get_direction());
            //    Serial.print("nextMove is: ");
            //    Serial.println(nextMove, DEC);

                mc.moveCardinal(nextMove);
                switch(mc.get_direction()) {
                    case dir::NORTH:
                        cell -= 7;
                        break;
                    case dir::EAST:
                        cell += 1;
                        break;
                    case dir::SOUTH:
                        cell += 7;
                        break;
                    case dir::WEST:
                        cell -= 1;
                        break;
                }
                Serial.write(cell);
            } else {
                // translate the character we received into a
                //    useable CARDINAL, make the move, then
                //    tell the upper half that we're ready
                //    for the next move
                mc.moveCardinal(ms.computeNextmove(synack));
                switch(mc.get_direction()) {
                    case dir::NORTH:
                        cell -= 7;
                        break;
                    case dir::EAST:
                        cell += 1;
                        break;
                    case dir::SOUTH:
                        cell += 7;
                        break;
                    case dir::WEST:
                        cell -= 1;
                        break;
                }
            //    Serial.write(cell);
                Serial.write(Configurator::OP_OK);
            }
        }
        marquee.display(cell);
        // this displays the current direction as an "integer"
        //    this should translate to:
        //
        //        NORTH -> 1 = 01
        //        EAST  -> 2 = 10
        //        SOUTH -> 3 = 11
        //        WEST  -> 4 = 00
        //
        marquee.direction(mc.get_direction());
        synack = 0x00;
    }
    
    if (configuration.lobatt()) {
        marquee.display(marquee.LO);
    }
}
