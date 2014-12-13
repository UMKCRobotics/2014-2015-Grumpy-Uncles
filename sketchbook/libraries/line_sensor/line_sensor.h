#include <QTRSensors.h>

//LED Thresholds
    /*int[] cMinOn; //white with LED on
    int[] cMaxOn; //black with LED on
    int[] cMinOff; //white with LED off
    int[] cMaxOff;  //black with LED off*/

    class LineSensors {
    /* this will be altered in the future.
       nothing in this class is set in stone
       and is subject to change based on the
       yet-to-be-acquired line sensor library. */
      private:
        int pin;
        int threshold;
        QTRSensorsRC bar;
        
      public:
        LineSensors(int constructed_threshold) {
          // some constructor
          threshold = constructed_threshold;
          //set_calibration(cMinOn,cMaxOn,cMinOff,cMaxOff);
        }
        
        void LS_Setup(){
           pinMode(pin, OUTPUT);
        }
        
        byte poll_sensors() {
          byte sensor_state = 0x00;
          unsigned int sensor_values[8];
          bar.read(sensor_values); //use readCalibrated if using calibration
          
          for (int ith = 0; ith < 8; ith++) {
            if (sensor_values[ith] > threshold) {
              sensor_state |= 0x01;
            }
            sensor_state << 1;
          }
          return(sensor_state);
        }
        
        void set_calibration(unsigned int* set_cMinOn, unsigned int* set_cMaxOn, unsigned int* set_cMinOff, unsigned int* set_cMaxOff){
         bar.calibratedMinimumOn = set_cMinOn;
         bar.calibratedMaximumOn = set_cMaxOn;
         bar.calibratedMinimumOff = set_cMinOff;
         bar.calibratedMaximumOff = set_cMaxOff;
        }
         
    };

