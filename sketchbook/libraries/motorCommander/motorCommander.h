class motor {
	private:
		byte pin_one;
		byte pin_two;
		bool forward;

		short fix_(short wrong) {
			return(map(wrong, 255, 0, 0, 255));
		}

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
			analogWrite(pin_one, fix_(throttle));
		}

		void go_reverse(byte throttle) {
			forward = false;
			digitalWrite(pin_one, HIGH);
			analogWrite(pin_two, fix_(throttle));
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
		#define SPEED_MIN 100
		#define SPEED_MAX 200

		dir::Cardinal current_direction;

		float rear_motor_ratio;
		short quarter_revolution;
		short quarter_turn;
		short one_foot;

		Encoder odo_left;
		Encoder odo_right;

		// am using defines here because the QTR library uses
		//    them, and when in rome...
		#define NUM_SENSORS 8
		#define TIMEOUT     2500
		#define EMITTER_PIN QTR_NO_EMITTER_PIN

		uint8_t sen_pins[NUM_SENSORS];
		unsigned int sen_values[NUM_SENSORS];
		LineSensors sen_bar;

		void set_throttle() {
			throttle = map(analogRead(throttle_pin), 0, 1023, 0, 255);
		}

	public:
		motorCommander() { 
			// YOU MUST ALWAYS PLACE THE ROBOT ON THE BOARD FACING NORTH!
			// NORTH is in the direction of smaller numbers;
			//    i.e., 48 -> 41 and 49 -> 42.
			current_direction = dir::NORTH;

			// this number comes from Darren's magic trig bag.
			// the idea here is that the rear motors are applied
			//    a fraction of the front wheel's power. this
			//    _should_ allow the bot to turn where we want.
			//    (in-between the front wheels)
			rear_motor_ratio = .70909;
			// through experimentation, we need to adjust by the
			//    difference (1 - rear_motor_ratio) to get the
			//    adjusted speed for the wheels.
			//rear_motor_ratio = .29191;


			// left side is backwards owing to pinning.
			odo_left.attach(27, 26);
			odo_right.attach(24, 25);

			// a full revolution of the encoder is 1200 ticks.
			quarter_revolution = 300;
			quarter_turn = 820;
			one_foot = 1926;
		}

		void init() {
			throttle_pin = A3;

			left_front.attach ( 7,  6);
			left_rear.attach  ( 4,  5);
			right_front.attach( 8, 10);
			right_rear.attach (12, 11);

			sen_bar.init();
		}

		void set_direction(dir::Cardinal new_direction) {
			current_direction = new_direction;
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

		void STOP() {
			left_front.stop();
			left_rear.stop();
			right_front.stop();
			right_rear.stop();
		}

		// the boolean is for testing. pass false to ignore
		//    any of the line following.
		// it's worth noting that any throttle value greater
		//    than 230 is reasonably too slow.
		// good testing value seems to be around 180.
		// it looks like 100 should be a good upper bound for
		//    fastness. anything less than 100 is too fast.
		//
		// fast / 0 < 100 < goldilocks < 230 < 255 / slow
		//
		// for roundiness, let's do: 100 < speed < 200
		void MOVE_FORWARD(bool follow = true) {
			short odo_old = (odo_left.read() + odo_right.read())/2;
			short current;
			// make sure we're actually stopped.
			STOP();

			// call out and see what the throttle is at.
			set_throttle();

			// this define needs a better home
			#define FAST_SCALING 1.01
			#define SLOW_SCALING 0.99

			// the initial push to begin moving forward.
			left_front.go_forward(throttle);
			left_rear.go_forward(throttle);
			right_front.go_forward(throttle);
			right_rear.go_forward(throttle);
			
			// if we can get the second sensor set in the front
			//    this shouldn't be needed anymore. the idea is
			//    that the front sensor will already be past the
			//    line and then the master stop condition will be
			//    looking for when the front reads white and the
			//    back reads black.
			//
			// this waits for the robot to move past a black line
			do {
				sen_bar.poll_sensors();
			} while(!sen_bar.white(0) || !sen_bar.white(7));
			
			short distance_moved;
			do {
head_of_loop:
				current = (odo_left.read() + odo_right.read())/2;
				distance_moved = abs(current-odo_old);
				sen_bar.poll_sensors();
				//Robot is on the black line and is center
				if (sen_bar.white(2) & sen_bar.white(5)){
					if (!sen_bar.white(3) && !sen_bar.white(4)){
						left_front.go_forward(throttle);
						left_rear.go_forward(throttle);
						right_front.go_forward(throttle);
						right_rear.go_forward(throttle);
					}
				//Robot is off course and needs to correct right
				} else if (!sen_bar.white(5)){
					right_front.go_forward(throttle * FAST_SCALING);
					right_rear.go_forward(throttle * FAST_SCALING);
					left_front.go_forward(throttle * SLOW_SCALING);
					left_rear.go_forward(throttle * SLOW_SCALING);
				//Robot is off course and needs to correct left
				} else if (!sen_bar.white(2)){
					left_front.go_forward(throttle * FAST_SCALING);
					left_rear.go_forward(throttle * FAST_SCALING);
					right_front.go_forward(throttle * SLOW_SCALING);
					right_rear.go_forward(throttle * SLOW_SCALING);
				}

				if (distance_moved < (one_foot * .9)){
					goto head_of_loop;
				}
			} while ((sen_bar.white(0) || sen_bar.white(7)));
				
			//Robot is back to black line and has moved forward a square
			STOP();
		}

		void TURN_LEFT() {
			//This using encoder data and a turn constant to properly turn
			bool stop_l = false;
			bool stop_r = false;
			int32_t old_l = odo_left.read();
			int32_t old_r = odo_right.read();
			int32_t current_l;
			int32_t current_r;
			int32_t speed_rear;

			set_throttle();
			// yes, this will demote the actual answer to an
			//    integer, but that's okay. we can afford to
			//    lose some precision on this.
//			speed_rear = throttle * rear_motor_ratio;
			// trying a different scaling method.
			// map the throttle into a 0(slow) -> 255(fast)
			//    slope, multiply the ratio in to get the
			//    adjusted speed for the rear motor.
			speed_rear = rear_motor_ratio * throttle;
			//             map(throttle, 255, 0, 0, 255);
			// then take that difference and add it into our
			//    motor's slope.
//			speed_rear += throttle;
			left_front.go_reverse(throttle);
			left_rear.go_reverse(speed_rear);
			right_front.go_forward(throttle);
			right_rear.go_forward(speed_rear);

//			Serial.print("MC :: TL --> throttle: ");
//			Serial.print(throttle, DEC);
//			Serial.print(", scaled: ");
//			Serial.print(speed_rear, DEC);
//			Serial.println();

			do {
				current_l = odo_left.read();
				current_r = odo_right.read();
				
				if (stop_l == false
				&& abs(current_l - old_l) >= quarter_turn) {
					left_front.stop();
					left_rear.stop();
					stop_l = true;
				}
				if (stop_r == false
				&& abs(current_r - old_r) >= quarter_turn) {
					right_front.stop();
					right_rear.stop();
					stop_r = true;
				}
			} while (!stop_l ||  !stop_r);

			current_direction--;

			STOP();
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
			int32_t speed_rear;

			set_throttle();
			// yes, this will demote the actual answer to an
			//    integer, but that's okay. we can afford to
			//    lose some precision on this.
//			speed_rear = throttle * rear_motor_ratio;
			speed_rear = rear_motor_ratio * throttle;
//			             map(throttle, 255, 0, 0, 255);
			// then take that difference and add it into our
			//    motor's slope.
//			speed_rear += throttle;
			left_front.go_forward(throttle);
			left_rear.go_forward(speed_rear);
			right_front.go_reverse(throttle);
			right_rear.go_reverse(speed_rear);

//			Serial.print("MC :: TR --> throttle: ");
//			Serial.print(throttle, DEC);
//			Serial.print(", scaled: ");
//			Serial.print(speed_rear, DEC);
//			Serial.println();


			do {
				current_l = odo_left.read();
				current_r = odo_right.read();

				if (stop_l == false
				&& abs(current_l - old_l) >= quarter_turn) {
					left_front.stop();
					left_rear.stop();
					stop_l = true;
				}
				if (stop_r == false
				&& abs(current_r - old_r) >= quarter_turn) {
					right_front.stop();
					right_rear.stop();
					stop_r = true;
				}
			} while (!stop_l ||  !stop_r);

			current_direction++;
			
			STOP();       
		}

		int get_odo_left() {
			return(odo_left.read());
		}

		int get_odo_right() {
			return(odo_right.read());
		}

};
