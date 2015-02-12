/* welcome to branch mixer.
 *
 * this is a second (date, 02/09/15) merge of both
 *    arduino and udoo branches of grumpy uncle.
 *
 * the primary branch is 'arduino'.
 *
 * for reference, look at the branch sandbox.
 */

#include <SPI.h>
#include <Segment.h>
#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>
#include <Cardinal.h>
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

const byte throttle_pin = A3;

void setup() {
    Serial.begin(115200);
    marquee.init();    
    configuration.initialize();
    mc.init(throttle_pin);
}

const char cmd_led = 0xE0;
boolean synched = false;
boolean reset_held = false;
unsigned long reset_wait = 0;
unsigned long release_time = 0;
const byte gopin = 41;

void loop() {
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
        marquee.display('o');
        marquee.light(LED::YELLOW);
        do {
            synack = Serial.read();
        } while (synack != Configurator::OP_SYN);
        Serial.print(Configurator::OP_ACK);
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
                Serial.print("nextMove is: ");
                Serial.println(nextMove, DEC);

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
//            while(mc.moveCardinal((dir::Cardinal)synack) != 0);
//            Serial.write(Configurator::OP_OK);
            }
        }
        marquee.display(cell);
        synack = 0x00;
    }
}
