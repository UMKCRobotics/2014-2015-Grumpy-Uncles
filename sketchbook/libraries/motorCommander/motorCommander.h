#include <Arduino.h>
//#include <Cardinal.h>
//#include <line_sensor.h>

/*
// Special behavior for ++Cardinal
Cardinal& operator++(Cardinal &c ) {
	c = static_cast<Cardinal>( static_cast<int>(c) + 1 );
	if ( c == Cardinal::END_OF_LIST )
	c = Cardinal::NORTH;
	return c;
}

// Special behavior for Cardinal++
Cardinal operator++(Cardinal &c, int ) {
	Cardinal result = c;
	++c;
	return result;
}
*/


class motor{
	private:
		int pin_one;
		int pin_two;

	public:
		motor() { };

		motor(int n_forward, int n_backward)
			:pin_one(n_forward), pin_two(n_backward) {};

		void M_Setup(int n_forward, int n_backward) {
			pin_one = n_forward;
			pin_two = n_backward;
			pinMode(pin_one, OUTPUT);
			pinMode(pin_two, OUTPUT);
		}

		void M_Setup(){
			pinMode(pin_one, OUTPUT);
			pinMode(pin_two, OUTPUT);
		}

		void go_forward(int mvmt_speed) {
			digitalWrite(pin_two, LOW);
			analogWrite(pin_one, mvmt_speed);
		}

		void go_backward(int mvmt_speed) {
			digitalWrite(pin_one, LOW);
			analogWrite(pin_two, mvmt_speed);
		}

		void stop_mvmt(){
			digitalWrite(pin_one, LOW);
			digitalWrite(pin_two, LOW);
		}

		void all_stop() {
			digitalWrite(pin_one, HIGH);
			digitalWrite(pin_two, HIGH);
		}

};

class motorCommander{
	private:
		motor MF_Left;
		motor MB_Left;
//		motor MF_Right;
//		motor MB_Right;

		int turn_delay90 = 1000;
		dir::Cardinal current_direction = dir::NORTH;

		LineSensors bar;
		byte throttle;
		byte pin_throttle;
		byte speed_l;
		byte speed_r;

		// these are only some of the valid states that are
		//    returned by poll_sensor. the bits indicate
		//    roughly where the line is under the sensor bar.
		static const byte BLANK = 0x00;
		static const byte ON_PATH = 0x18;
		static const byte GUIDE_LEFT = 0x38;
		static const byte GUIDE_RIGHT = 0x1C;
		static const byte SLIDE_LEFT = 0x78;
		static const byte SLIDE_RIGHT = 0x1E;
		static const byte FULL_LINE = 0xFF;

	public:
		motorCommander() {
		}

		void mcSetup(byte pin_t) {
			pin_throttle = pin_t;
			bar.init(500);

			// IAW the DRV8833 spec sheet, the order
			//    of constructors is (*IN1, *IN2),
			MF_Left.M_Setup(8, 9);
			MB_Left.M_Setup(6,  7);
//			MF_Right.M_Setup(4, 5);
//			MB_Right.M_Setup(6, 7);
			STOP();
//			Serial.println("MC --> setup done");
		}

		void set_direction(dir::Cardinal n_dir) {
			current_direction = n_dir;
		}

		dir::Cardinal get_direction() {
			return current_direction;
		}

		void MOVE_FORWARD() {
			STOP();
			throtte = map(analogRead(pin_throttle), 0, 1023, 0, 255);
			speed_l = speed_r = throttle;
			// RYAN: need to have a better function for speed
			//       control. what if you're going speed '5', how
			//       do you subtract 10.
			//       look at division or exponential.
			byte sensors = BLANK;
			int offset;
		//	offset = bar.read_line();
		//	offset = offset / 10;
			offset = 0;
			do {
				MF_Left.go_forward(speed_l);
				MB_Left.go_forward(speed_l);
//				MF_Right.go_forward(speed_r);
//				MB_Right.go_forward(speed_r);

				if (offset < 0) {
					// drifting to the left.
					speed_l = throttle + offset;
					speed_r = throttle - offset;
				} else if (offset > 0) {
					// drifting to the right.
					speed_l = throttle - offset;
					speed_r = throttle + offset;
				} else {
					// right on target
					// do not adjust speed.
				}
			} while (bar.poll_sensors() != FULL_LINE);

			MF_Left.go_forward(throttle);
			MB_Left.go_forward(throttle);
//			MF_Right.go_forward(throttle);
//			MB_Right.go_forward(throttle);
			// delay for moment to get past the line and
			//    more into the center of the cell.
			delay(250);
			// and then stop
			STOP();
		}

		void MOVE_BACKWARD() {
			STOP();
			throtte = map(analogRead(pin_throttle), 0, 1023, 0, 255);
			speed_l = speed_r = throttle;
			// RYAN: need to have a better function for speed
			//       control. what if you're going speed '5', how
			//       do you subtract 10.
			//       look at division or exponential.
			byte sensors = BLANK;
			int offset;
		//	offset = bar.read_line();
		//	offset = offset / 10;
			offset = 0;
			do {
				MF_Left.go_backward(speed_l);
				MB_Left.go_backward(speed_l);
//				MF_Right.go_backward(speed_r);
//				MB_Right.go_backward(speed_r);

				if (offset < 0) {
					// drifting to the left.
					speed_l = throttle + offset;
					speed_r = throttle - offset;
				} else if (offset > 0) {
					// drifting to the right.
					speed_l = throttle - offset;
					speed_r = throttle + offset;
				} else {
					// right on target
					// do not adjust speed.
				}
			} while (bar.poll_sensors() != FULL_LINE);

			MF_Left.go_backward(throttle);
			MB_Left.go_backward(throttle);
//			MF_Right.go_backward(throttle);
//			MB_Right.go_backward(throttle);
			// delay for moment to get past the line and
			//    more into the center of the cell.
			delay(250);
			// and then stop
			STOP();
		}

		void TURN_RIGHT() {
			STOP();
//			MF_Right.go_backward(throttle);
			MF_Left.go_forward(throttle);
			delay(turn_delay90); 
			STOP();
		}

		void TURN_LEFT() {
			STOP();
//			MF_Right.go_forward(throttle);
			MF_Left.go_backward(throttle);
			delay(turn_delay90); 
			STOP();
		}

		void STOP(){
			MF_Left.stop_mvmt();
			MB_Left.stop_mvmt();
//			MF_Right.stop_mvmt();
//			MB_Right.stop_mvmt();
		}

		short moveCardinal(dir::Cardinal direction_input) {
			int desired_direction = current_direction - direction_input;
			switch (desired_direction){
				case 0:
					MOVE_FORWARD();
					break;
				case 1: case -3:
					TURN_LEFT();
					current_direction--;
					break;
				case 2: case -2:
					TURN_RIGHT();
					current_direction++;
					break;
				case 3: case -1:
					TURN_RIGHT();
					current_direction++;
					break;
			}

			switch(current_direction){
				case -1:
					current_direction = 3;
					break;
				case -2:
					current_direction = 2;
					break;
				case -3:
					current_direction = 1;
					break;
				case -4: case 4:	
					current_direction = 0;
					break;
			}

			return(desired_direction);
		}
};
