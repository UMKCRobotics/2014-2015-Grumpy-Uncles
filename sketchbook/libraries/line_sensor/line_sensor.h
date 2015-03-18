
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


	public:
		static const unsigned char NUM_SENSORS = 8;
		static const unsigned short TIMEOUT = 2500;
		static const unsigned char LINE_FULL = 0xFF;

	private:
		#define EMITTER_PIN QTR_NO_EMITTER_PIN
//		unsigned char pins[8];
		unsigned int sensor_values[NUM_SENSORS];
		int threshold;
		QTRSensorsRC bar;

	public:
		LineSensors() {
			//default constructor
			//used to set pins up here, moved to init so pins can be set per bar
		}
		void init(const unsigned char pins[], int constructed_threshold = TIMEOUT){
/*			//FRONT
			//NEED PIN VALUES BELOW FOR NEW FRONT SENSOR BAR
			if (set = 0){
				//set_calibration(cMinOn,cMaxOn,cMinOff,cMaxOff);
				pins[0] = 43;	sensor_values[0] = 0;
				pins[1] = 45;	sensor_values[1] = 0;
				pins[2] = 47;	sensor_values[2] = 0;
				pins[3] = 42;	sensor_values[3] = 0;
				pins[4] = 44;	sensor_values[4] = 0;
				pins[5] = 46;	sensor_values[5] = 0;
				pins[6] = 29;	sensor_values[6] = 0;
				pins[7] = 28;	sensor_values[7] = 0;
			} else {
			//REAR
				//set_calibration(cMinOn,cMaxOn,cMinOff,cMaxOff);
				pins[0] = 14;	sensor_values[0] = 0;
				pins[1] = 15;	sensor_values[1] = 0;
				pins[2] = 16;	sensor_values[2] = 0;
				pins[3] = 17;	sensor_values[3] = 0;
				pins[4] = 18;	sensor_values[4] = 0;
				pins[5] = 19;	sensor_values[5] = 0;
				pins[6] = 34;	sensor_values[6] = 0;
				pins[7] = 35;	sensor_values[7] = 0;
			}
*/
			sensor_values[0] = 0;
			sensor_values[1] = 0;
			sensor_values[2] = 0;
			sensor_values[3] = 0;
			sensor_values[4] = 0;
			sensor_values[5] = 0;
			sensor_values[6] = 0;
			sensor_values[7] = 0;
			threshold = constructed_threshold;
			bar.init(pins, NUM_SENSORS, threshold, EMITTER_PIN);
			//	bar.calibrate();
		}

		bool white(const int index){
			if (value(index) < threshold){
				return true;
			} else {
				return false;
			}
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
