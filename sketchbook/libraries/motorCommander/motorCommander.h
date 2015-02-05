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

		void callback() {
			stop();
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

		short quarter_turn = 280;

		Encoder<motor> odo_left;
		Encoder<motor> odo_right;

		void set_throttle() {
			throttle = map(analogRead(throttle_pin), 0, 1023, 255, 0);
		}

	public:
		motorCommander() { 
			// left side is backwards owing to pinning.
			odo_left.attach(27, 26);
			odo_right.attach(24, 25);
		}

		void init(byte pin_t) {
			throttle_pin = pin_t;
			left_front.attach ( 7,  6);
			left_rear.attach  ( 4,  5);
			right_front.attach( 8, 10);
			right_rear.attach (12, 11);

			odo_left.register_callback(quarter_turn, &left_front);
			odo_right.register_callback(quarter_turn, &right_front);
		}

		void MOVE_FORWARD() {
			STOP();
			set_throttle();
			speed_l = throttle;

			left_front.go_forward(speed_l);
			left_rear.go_forward(speed_l);
			right_front.go_forward(speed_l);
			right_rear.go_forward(speed_l);

			delay(1000);
			// AS SOON AS YOU HIT THE LINE, STOP.
			STOP();
		}

		void MOVE_BACKWARD() {
			STOP();
			set_throttle();
			speed_l = throttle;

			left_front.go_reverse(speed_l);
			left_rear.go_reverse(speed_l);
			right_front.go_reverse(speed_l);
			right_rear.go_reverse(speed_l);

			delay(1000);
			// AS SOON AS YOU HIT THE LINE, STOP.
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
		}

		int get_odo_left() {
			return(odo_left.read());
		}

		int get_odo_right() {
			return(odo_right.read());
		}

};
