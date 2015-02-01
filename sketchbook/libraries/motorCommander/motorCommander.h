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
		byte throttle_pin;
		byte throttle;
		byte speed_l;

	public:
		motorCommander() { };

		void init(byte pin_t) {
			throttle_pin = pin_t;
			left_front.attach(12, 11);
			left_rear.attach(8, 10);
		}

		void MOVE_FORWARD() {
			STOP();
			throttle = map(analogRead(throttle_pin), 0, 1023, 0, 255);
			speed_l = throttle;

			left_front.go_forward(speed_l);
			left_rear.go_forward(speed_l);

			delay(1000);
			STOP();
		}

		void MOVE_BACKWARD() {
			STOP();
			throttle = map(analogRead(throttle_pin), 0, 1023, 0, 255);
			speed_l = throttle;

			left_front.go_reverse(speed_l);
			left_rear.go_reverse(speed_l);

			delay(1000);
			STOP();
		}

		void STOP() {
			left_front.stop();
			left_rear.stop();
		}
};
