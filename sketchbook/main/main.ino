#include <arduinoConfig.h>

#include <Cardinal.h>
#include <worldSensor.h>
#include <mazeSolver.h>

#include <motorCommander.h>
//
//
//
//#include "/home/umkc/robot/arduino/2014-2015-Grumpy-Uncles/sketchbook/libraries/motorCommander/motorCommander.h"
//#include "/home/umkc/robot/arduino/2014-2015-Grumpy-Uncles/sketchbook/libraries/mazeSolver/mazeSolver.h"

motorCommander mc;
mazeSolver ms;

dir::Cardinal nextMove;
Configurator* configuration;

void setup(){
    Serial.begin(115200);
    mc.mcSetup();
    
    // create configuration object and collect data.
    configuration = new Configurator;
    configuration->setRound();
    configuration->setPart();
    configuration->setThrottle();
    
    configuration->sendData(&Serial);
}

void loop(){
//    nextMove = ms.computeNextmove(mc.get_direction());
     mc.moveCardinal(nextMove);
//    Serial.write(nextMove);
}
