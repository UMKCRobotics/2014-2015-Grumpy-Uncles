class motor {
	private:
		byte pin_one;
		byte pin_two;
		bool forward;

	public:
		motor() { };

		void attach(byte pin_o, byte pin_t) {
			pin_one = pin_o;
			pin_two = pin_t;
			forward = true;

			pinMode(pin_one, OUTPUT);
			pinMode(pin_two, OUTPUT);
		}

		void go_forward(byte throttle) {
			forward = true;
			digitalWrite(pin_two, HIGH);
			analogWrite(pin_one, throttle);
		}

		void go_reverse(byte throttle) {
			forward = false;
			digitalWrite(pin_one, HIGH);
			analogWrite(pin_two, throttle);
		}

		void stop() {
			if (forward) {
				digitalWrite(pin_two, HIGH);
				analogWrite(pin_one, 0xFF);
			} else {
				digitalWrite(pin_one, HIGH);
				analogWrite(pin_two, 0xFF);
			}
		}
};

class motorCommander {
	private:
		motor left_front;
		motor left_rear;
		motor right_front;
		motor right_rear;
		byte throttle_pin;
		byte throttle;
		byte speed_l;
		byte speed_r;

		dir::Cardinal current_direction;

		short quarter_turn;
		short one_foot;

		Encoder odo_left;
		Encoder odo_right;

		#define NUM_SENSORS 8
		#define TIMEOUT     2500
		#define EMITTER_PIN QTR_NO_EMITTER_PIN

		uint8_t sen_pins[NUM_SENSORS];
		unsigned int sen_values[NUM_SENSORS];
		QTRSensorsRC sen_bar;

		void set_throttle() {
			throttle = map(analogRead(throttle_pin), 0, 1023, 255, 0);
		}

	public:
		motorCommander() { 
			// left side is backwards owing to pinning.
			odo_left.attach(27, 26);
			odo_right.attach(24, 25);
			current_direction = dir::NORTH;

			quarter_turn = 820;
			one_foot = 1926;
		}

		dir::Cardinal get_direction() {
			return current_direction;
		}

		short moveCardinal(dir::Cardinal move_to) {
			short desired_direction = current_direction - move_to;
		//	Serial.print("Turn difference: ");
		//	Serial.println(desired_direction, DEC);

            switch (desired_direction){
                case 0:
                    MOVE_FORWARD();
                    break;
                case -3: case 1:
                    TURN_LEFT();
                    MOVE_FORWARD();
                    break;
                case -1: case 3:
                    TURN_RIGHT();
                    MOVE_FORWARD();
                    break;
                case -2: case 2:
                    TURN_RIGHT();
                    TURN_RIGHT();
                    MOVE_FORWARD();
                    break;
            }

		//	Serial.print("I am facing: ");
		//	Serial.print(current_direction, DEC);
		//	Serial.println();
            return(desired_direction);
	    }

		void init(byte pin_t) {
			throttle_pin = pin_t;

			left_front.attach ( 7,  6);
			left_rear.attach  ( 4,  5);
			right_front.attach( 8, 10);
			right_rear.attach (12, 11);

			sen_bar.init(sen_pins, NUM_SENSORS, TIMEOUT, EMITTER_PIN);
		}

		void MOVE_FORWARD() {
			STOP();
			set_throttle();
			speed_l = throttle;

			int32_t old_odo = (odo_left.read() + odo_right.read()) / 2;
			int32_t current_odo;
			bool stopped = false;

			left_front.go_forward(speed_l);
			left_rear.go_forward(speed_l);
			right_front.go_forward(speed_l);
			right_rear.go_forward(speed_l);

			do {
				// take an average of the current readings.
				current_odo = (odo_left.read() + odo_right.read()) / 2;
			} while ((current_odo - old_odo) < one_foot);
			// using encoder ticks for now.
			// can't get the line sensor to work.
			STOP();
		}

		void MOVE_BACKWARD() {
			STOP();
			set_throttle();
			speed_l = throttle;

			int32_t old_odo = (odo_left.read() + odo_right.read()) / 2;
			int32_t current_odo;
			bool stopped = false;

			left_front.go_reverse(speed_l);
			left_rear.go_reverse(speed_l);
			right_front.go_reverse(speed_l);
			right_rear.go_reverse(speed_l);

			do {
				// take an average of the current readings.
				current_odo = (odo_left.read() + odo_right.read()) / 2;
			} while ((old_odo - current_odo) < one_foot);
			// using encoder ticks for now.
			// can't get the line sensor to work.
			STOP();
		}

		void STOP() {
			left_front.stop();
			left_rear.stop();
			right_front.stop();
			right_rear.stop();
		}

		void TURN_LEFT() {
			// will delay by a set amount for now.
			// need to use encoder data.
			bool stop_l = false;
			bool stop_r = false;
			int32_t old_l = odo_left.read();
			int32_t old_r = odo_right.read();
			int32_t current_l;
			int32_t current_r;

			set_throttle();
			left_front.go_reverse(throttle);
			right_front.go_forward(throttle);

			do {
				current_l = odo_left.read();
				current_r = odo_right.read();

				if (stop_l == false
				&& (old_l - current_l) >= quarter_turn) {
					left_front.stop();
					stop_l = true;
				}
				if (stop_r == false
				&& (current_r - old_r) >= quarter_turn) {
					right_front.stop();
					stop_r = true;
				}
			} while (!(stop_l && stop_r));

			current_direction--;
		}

		void TURN_RIGHT() {
			// will delay by a set amount for now.
			// need to use encoder data.
			bool stop_l = false;
			bool stop_r = false;
			int32_t old_l = odo_left.read();
			int32_t old_r = odo_right.read();
			int32_t current_l;
			int32_t current_r;

			set_throttle();
			left_front.go_forward(throttle);
			right_front.go_reverse(throttle);

			do {
				current_l = odo_left.read();
				current_r = odo_right.read();

				if (stop_l == false
				&& (current_l - old_l) >= quarter_turn) {
					left_front.stop();
					stop_l = true;
				}
				if (stop_r == false
				&& (old_r - current_r) >= quarter_turn) {
					right_front.stop();
					stop_r = true;
				}
			} while (!(stop_l && stop_r));

            current_direction++;
		}

		int get_odo_left() {
			return(odo_left.read());
		}

		int get_odo_right() {
			return(odo_right.read());
		}

};
