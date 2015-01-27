#include <Arduino.h>
//#include <Cardinal.h>
//#include <line_sensor.h>

#define DEFAULT_SPEED 150

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
			analogWrite(pin_two, LOW);
			//slow code here
			analogWrite(pin_one, mvmt_speed);
		}

		void go_backward(int mvmt_speed) {
			analogWrite(pin_one, LOW);
			//slow code here
			analogWrite(pin_two, mvmt_speed);
		}

		void stop_mvmt(){
			analogWrite(pin_one, LOW);
			analogWrite(pin_two, LOW);
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
//		motor* MB_Left;
//		motor* MF_Right;
//		motor* MB_Right;

		int turn_delay90 = 1000;
		dir::Cardinal current_direction = dir::NORTH;

		LineSensors* bar;
		byte throttle;
		byte speed_l;
		byte speed_r;

		static const byte BLANK = 0x00;
		static const byte ON_PATH = 0x18;
		static const byte GUIDE_LEFT = 0x38;
		static const byte GUIDE_RIGHT = 0x1C;
		static const byte SLIDE_LEFT = 0x78;
		static const byte SLIDE_RIGHT = 0x1E;
		static const byte FULL_LINE = 0xFF;

	public:
		motorCommander() {
			// IAW the DRV8833 spec sheet, the order
			//    of constructors is (*IN1, *IN2),
//			MF_Left  = new motor(11, 10);
//			MB_Left  = new motor( 8,  9);
//			MF_Right = new motor( 7,  6);
//			MB_Right = new motor( 4,  5);
		}

		~ motorCommander() {
//			delete MF_Left;
//			delete MB_Left;
//			delete MF_Right;
//			delete MB_Right;
		}

		void mcSetup(byte i_throttle) {
			throttle = i_throttle;
			bar = NULL;

			MF_Left.M_Setup(11, 10);
			MB_Left.M_Setup( 8,  9);
//			MF_Left->M_Setup();
//			MB_Left->M_Setup();
//			MF_Right->M_Setup();
//			MB_Right->M_Setup();
			STOP();
			Serial.println("MC --> setup done");
		}

		void set_direction(dir::Cardinal n_dir) {
			current_direction = n_dir;
		}

		dir::Cardinal get_direction() {
			return current_direction;
		}

//		void MOVE_FORWARD(int movement_speed = DEFAULT_SPEED) {
		void MOVE_FORWARD() {
			byte sensors = BLANK;
			int offset;
			offset = (bar? bar->read_line() : 0);
			offset = offset / 10;
			do {
				MF_Left.go_forward(speed_l);
				MB_Left.go_forward(speed_l);
//				MF_Right->go_forward(speed_r);
//				MB_Right->go_forward(speed_r);

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
			} while (bar? bar->poll_sensors() != FULL_LINE : false);

			MF_Left.go_forward(throttle);
			MB_Left.go_forward(throttle);
//			MF_Right->go_forward(throttle);
//			MB_Right->go_forward(throttle);
			// delay for a quarter of a second
			delay(250);
			// and then stop
			STOP();
		}

//		void MOVE_BACKWARD(int movement_speed = DEFAULT_SPEED){
		void MOVE_BACKWARD() {
			byte sensors = BLANK;
			int offset;
			offset = (bar? bar->read_line() : 0);
			offset = offset / 10;
			do {
//				MF_Left->go_backward(speed_l);
//				MB_Left->go_backward(speed_l);
//				MF_Right->go_backward(speed_r);
//				MB_Right->go_backward(speed_r);

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
			} while (bar? bar->poll_sensors() != FULL_LINE : 0);

//			MF_Left->go_backward(throttle);
//			MB_Left->go_backward(throttle);
//			MF_Right->go_backward(throttle);
//			MB_Right->go_backward(throttle);
			// delay for a quarter of a second
			delay(250);
			// and then stop
			STOP();
		}

//		void TURN_RIGHT(int movement_speed = DEFAULT_SPEED){
		void TURN_RIGHT() {
			STOP();
//			MF_Right->go_backward(throttle);
//			MF_Left->go_forward(throttle);
			delay(turn_delay90); 
			STOP();
		}

//		void TURN_LEFT(int movement_speed = DEFAULT_SPEED){
		void TURN_LEFT() {
			STOP();
//			MF_Left->go_backward(throttle);
//			MF_Right->go_forward(throttle);
			delay(turn_delay90); 
			STOP();
		}

		void STOP(){
			MF_Left.stop_mvmt();
			MB_Left.stop_mvmt();
//			MF_Right->stop_mvmt();
//			MB_Right->stop_mvmt();
		}

		void moveCardinal(dir::Cardinal direction_input) {
			int desired_direction = current_direction - direction_input;
			switch (desired_direction){
				case 0:
					MOVE_FORWARD();
					break;
				case 1: case -3:
					TURN_LEFT();
					MOVE_FORWARD();
					break;
				case 2: case -2:
					TURN_RIGHT();
					TURN_RIGHT();
					MOVE_FORWARD();
					break;
				case 3: case -1:
					TURN_RIGHT();
					MOVE_FORWARD();
				break;
			}
			current_direction = direction_input;
		}
};
