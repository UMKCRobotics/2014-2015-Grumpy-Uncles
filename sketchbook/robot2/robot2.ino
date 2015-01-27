#include <motorCommander.h>

#include <worldSensor.h>
#include <mazeSolver.h>

#include <Cardinal.h>

#include <arduinoConfig.h>

byte throttle, c_round, c_part;
char synack;
dir::Cardinal nextMove;
Configurator configuration;
motorCommander mc;
mazeSolver ms;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    configuration.initialize();
    configuration.setRound();
    configuration.setPart();
    configuration.setThrottle();
    // this blocks, waiting for the UDOO to boot
    configuration.sendConfig(&Serial);
    
    mc.mcSetup();
}

void loop() {
    if (configuration.getPart() == 1) {
        do {
            synack = Serial.read();
        // wait until the UDOO says move.
        } while (synack != Configurator::OP_MOVE);
        // call worldsensor;
        nextMove = ms.computeNextmove(mc.get_direction());
        Serial.write(nextMove);
    } else {
        while (Serial.available() == 0);
        // read the next cardinal from the UDOO
        nextMove = dir::returnCardinal(Serial.read());
    }
    mc.moveCardinal(nextMove);
    Serial.write(Configurator::OP_OK);
}
