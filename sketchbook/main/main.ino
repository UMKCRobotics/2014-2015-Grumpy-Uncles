

#include "/home/umkc/robot/arduino/2014-2015-Grumpy-Uncles/sketchbook/libraries/motorCommander/motorCommander.h"
#include "/home/umkc/robot/arduino/2014-2015-Grumpy-Uncles/sketchbook/libraries/mazeSolver/mazeSolver.h"

motorCommander mc;
mazeSolver ms;

dir::Cardinal nextMove;

void setup(){
    Serial.begin(115200);
    mc.mcSetup();
}

void loop(){
    nextMove = ms.computeNextmove(mc.get_direction());
    mc.moveCardinal(nextMove);
    Serial.write(nextMove);
}
