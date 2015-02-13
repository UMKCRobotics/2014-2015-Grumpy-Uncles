
//#include<QTRSensors.h>

/****
//LEDThresholds
int[] cMinOn; //white with LED on
int[] cMaxOn; //black with LED on
int[] cMinOff; //white with LED off
int[] cMaxOff;  //black with LED off
****/

class LineSensors {
	/* this will be altered in the future.
	nothing in this class is set in stone
	and is subject to change based on the
	yet-to-be-acquired line sensor library. */
	private:
		unsigned char pins[8];
		unsigned int sensor_values[8];
		int pin;
		int threshold;
		QTRSensorsRC bar;

	public:
		LineSensors() {
			//set_calibration(cMinOn,cMaxOn,cMinOff,cMaxOff);
			pins[0] = 14;	sensor_values[0] = 0;
			pins[1] = 15;	sensor_values[1] = 0;
			pins[2] = 16;	sensor_values[2] = 0;
			pins[3] = 17;	sensor_values[3] = 0;
			pins[4] = 18;	sensor_values[4] = 0;
			pins[5] = 19;	sensor_values[5] = 0;
			pins[6] = 20;	sensor_values[6] = 0;
			pins[7] = 21;	sensor_values[7] = 0;
		}

		void init(int constructed_threshold){
			bar.init(pins, 8);
			//	bar.calibrate();
			threshold = constructed_threshold;
			//pinMode(pin, OUTPUT);
		}

		byte poll_sensors() {
			byte sensor_state = 0x00;
			bar.read(sensor_values); //use readCalibrated if using calibration

			for (int ith = 0; ith < 8; ith++) {
				if (sensor_values[ith] > threshold) {
					sensor_state |= 0x01;
				}
				sensor_state << 1;
			}
			return(sensor_state);
		}

		int value(const int index) {
			return (sensor_values[index]);
		}
		
		int read_line() {
			int line = bar.readLine(sensor_values);
			// 3500 is the mean return of the 8 sensor bar
			line = line - 3500;
			return (line);
		}

		void set_calibration(unsigned int* set_cMinOn,
		                     unsigned int* set_cMaxOn,
		                     unsigned int* set_cMinOff,
		                     unsigned int* set_cMaxOff){
			bar.calibratedMinimumOn = set_cMinOn;
			bar.calibratedMaximumOn = set_cMaxOn;
			bar.calibratedMinimumOff = set_cMinOff;
			bar.calibratedMaximumOff = set_cMaxOff;
		}

};
