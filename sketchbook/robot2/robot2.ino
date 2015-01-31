#include <motorCommander.h>

#include <worldSensor.h>
#include <mazeSolver.h>

#include <Cardinal.h>

#include <arduinoConfig.h>

byte throttle, c_round, c_part;
char synack;
dir::Cardinal nextMove;
short cell;
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
    switch(configuration.getRound()){
      case 1:
        cell = 48;
        break;
      case 2: case 3:
        cell = 49;
        break;
    }
    
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
    if (mc.moveCardinal(nextMove) == 0) {
      //do cell math
      switch(mc.get_direction()) {
        case dir::NORTH:
          cell += 7;
          break;
        case dir::EAST:
          cell += 1;
          break;
        case dir::SOUTH:
          cell -= 7;
          break;
        case dir::WEST:
          cell -= 1;
          break;
      }
    } // else, do nothing.
    
    Serial.write(cell);
}
