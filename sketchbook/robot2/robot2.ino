#include <worldSensor.h>
#include <mazeSolver.h>

#include <Cardinal.h>

#include <arduinoConfig.h>

byte throttle, c_round, c_part;
Configurator configuration;
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    configuration.initialize();
    configuration.setRound();
    configuration.setPart();
    configuration.setThrottle();
    // this blocks, waiting for the UDOO to boot
    configuration.sendConfig(&Serial);
}
char synack;
dir::Cardinal nextMove;

void loop() {
    if (configuration.getPart() == 1) {
        do {
            synack = Serial.read();
        // wait until the UDOO says move.
        } while (synack != Configurator::OP_MOVE);
        // call worldsensor;
        nextMove = computeNextMove(listofCardinals);
    } else {
        while (Serial.available() == 0);
        // read the next cardinal from the UDOO
        nextMove = Serial.read();
    }
    
    moveCardinal(nextMove);
    Serial.write(Configurator::OP_OK);
}
