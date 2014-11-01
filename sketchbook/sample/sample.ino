void setup() {
    Serial.begin(9600);
}
#include <Cardinal.h>

#include <motor_controller.h>

#include <ir_sensor.h>

#include <worldSensor.h>

#include <mazeSolver.h>

mazeSolver ms;
Drive_Sys drivetrain;

void loop() {

    ms.computeNextmove(dir::NORTH);
    
    Serial.println("test of mazeSolver");
}
