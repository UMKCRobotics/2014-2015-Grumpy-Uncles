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

		LineSensors F_sen_bar;
		LineSensors R_sen_bar;
		const unsigned char F_sen_pins[LineSensors::NUM_SENSORS] = { 43, 45, 47, 42, 44, 46, 29, 28 };
		const unsigned char R_sen_pins[LineSensors::NUM_SENSORS] = { 14, 15, 16, 17, 18, 19, 34, 35 };

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

		void init(int F_thresh, int R_thresh) {
			throttle_pin = A3;

			left_front.attach ( 7,  6);
			left_rear.attach  ( 4,  5);
			right_front.attach( 8, 10);
			right_rear.attach (12, 11);

			F_sen_bar.init(F_sen_pins, F_thresh);
			R_sen_bar.init(R_sen_pins, R_thresh);
		}

		void set_direction(dir::Cardinal new_direction) {
			current_direction = new_direction;
		}

		dir::Cardinal get_direction() {
			return current_direction;
		}

		short moveCardinal(dir::Cardinal move_to) {
			short desired_direction = current_direction - move_to;
			
			#ifdef GDEBUG
		//	Serial.print("Turn difference: ");
		//	Serial.println(desired_direction, DEC);
			#endif

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

            return(desired_direction);
	    }

		void STOP() {
			left_front.stop();
			left_rear.stop();
			right_front.stop();
			right_rear.stop();
		}

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
			// this waits for the rear sensor to move past
			//    the first black line (where we stopped)
			do {
				R_sen_bar.poll_sensors();
				F_sen_bar.poll_sensors();
			} while(!R_sen_bar.white(0) || !R_sen_bar.white(7));
			
			short distance_moved;
			bool stop_condition = false;
			bool next_line_check = false;
			do {
head_of_loop:
				current = (odo_left.read() + odo_right.read())/2;
				distance_moved = abs(current-odo_old);
				#ifdef GDEBUG
//				Serial.print("Distance Moved: ");
//				Serial.print(distance_moved, DEC);
				#endif
				F_sen_bar.poll_sensors();
				R_sen_bar.poll_sensors();

				#ifdef GDEBUG
//				Serial.print("MC :: FOR --> Front[ ");
//				for (int sth = 0; sth < 8; sth++) {
//					Serial.print(F_sen_bar.white(sth), DEC);
//					Serial.print(" ");
//				}
//				Serial.println("]");
//				Serial.print("MC :: FOR -->  Rear[ ");
//				for (int sth = 0; sth < 8; sth++) {
//					Serial.print(R_sen_bar.white(sth), DEC);
//					Serial.print(" ");
//				}
//				Serial.println("]");
				#endif

				float skew = (R_sen_bar.read_line() + F_sen_bar.read_line()) / 2;
				#ifdef GDEBUG
				Serial.print("MC :: FOR --> throttle: ");
				Serial.print(throttle, DEC);
				Serial.print(", skew: ");
				Serial.print(skew, DEC);
				Serial.print(" ("); Serial.print(R_sen_bar.read_line()); Serial.print(")");
				Serial.print(" ("); Serial.print(F_sen_bar.read_line()); Serial.print(")");
				#endif
				skew /= 100;
				#ifdef GDEBUG
				Serial.print(" -> ");
				Serial.print(skew, DEC);
				#endif
				// adjust the throttle by a percentage between
				//    what should be 1 and 9;
				skew = (float)throttle * (float)(skew / 100) * 2;
				#ifdef GDEBUG
				Serial.print(" -> ");
				Serial.print(skew, DEC);
				Serial.println();
				#endif

				left_front.go_forward(throttle - skew);
				left_rear.go_forward(throttle - skew);
				right_front.go_forward(throttle + skew);
				right_rear.go_forward(throttle + skew);

				// this should be the last thing to check
				// this will help to keep the robot moving
				//    past the bumps and small gaps.
				if (distance_moved < one_foot * .9) {
					goto head_of_loop;
				} else if (distance_moved > one_foot) {
					stop_condition = true;
				}

				if ((F_sen_bar.poll_sensors() & 0x81) == 0x81) {
					next_line_check = true;
				}

				if (next_line_check && ((R_sen_bar.poll_sensors() & 0x81) == 0x81)) {
					stop_condition = true;
				}
			} while (stop_condition == false);
//			} while (R_sen_bar.poll_sensors() == 0x81;
				
			//Robot is back to black line and has moved forward a square
			#ifdef GDEBUG
			Serial.println("Back to black line");
			Serial.println("Stopped");
			#endif
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

			#ifdef GDEBUG
			Serial.print("MC :: TL --> throttle: ");
			Serial.print(throttle, DEC);
			Serial.print(", scaled: ");
			Serial.print(speed_rear, DEC);
			Serial.println();
			#endif

			bool on_white_check = false;
			bool f_line_check = false;
			bool r_line_check = false;
			bool stop_condition = false;
			do {
				current_l = odo_left.read();
				current_r = odo_right.read();
				F_sen_bar.poll_sensors();
				R_sen_bar.poll_sensors();

				#ifdef GDEBUG
				Serial.print("MC :: TL --> Front[ ");
				for (int sth = 0; sth < 8; sth++) {
					Serial.print(F_sen_bar.white(sth), DEC);
					Serial.print(" ");
				}
				Serial.print("]");
				Serial.print("  Rear[ ");
				for (int sth = 0; sth < 8; sth++) {
					Serial.print(R_sen_bar.white(sth), DEC);
					Serial.print(" ");
				}
				Serial.println("]");
				#endif

				if (F_sen_bar.white(7) == false) {
					f_line_check = true;
				}

				if (f_line_check == true && F_sen_bar.white(7) == true) {
					on_white_check = true;
				}

				if (on_white_check == true && R_sen_bar.white(0) == false) {
					r_line_check = true;
				}

				if (r_line_check == true && R_sen_bar.white(0) == true) {
					stop_condition = true;
				}
				
				if (
					r_line_check == true && (
						F_sen_bar.white(0) && F_sen_bar.white(7)
					) || (
						R_sen_bar.white(0) && R_sen_bar.white(7)
					)
				) {
					stop_condition = true;
				}

				if (abs(old_l - current_l) > quarter_turn) {
					left_front.stop();
					left_rear.stop();
					stop_l = true;
				}

				if (abs(current_r - old_r) > quarter_turn) {
					right_front.stop();
					right_rear.stop();
					stop_r = true;
				}

				if (stop_l == true && stop_r == true) {
					stop_condition = true;
				}
			} while (stop_condition == false);
			current_direction--;
			STOP();
			#ifdef GDEBUG
			Serial.println("MC :: TL --> STOPPED");
			#endif
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

			#ifdef GDEBUG
//			Serial.print("MC :: TR --> throttle: ");
//			Serial.print(throttle, DEC);
//			Serial.print(", scaled: ");
//			Serial.print(speed_rear, DEC);
//			Serial.println();
			#endif

			bool on_white_check = false;
			for (;;){

				current_l = odo_left.read();
				current_r = odo_right.read();

				#ifdef GDEBUG
				Serial.print("MC :: TRight --> Front[ ");
				for (int sth = 0; sth < 8; sth++) {
					Serial.print(F_sen_bar.white(sth), DEC);
					Serial.print(" ");
				}
				Serial.println("]");
				Serial.print("MC :: TRight -->  Rear[ ");
				for (int sth = 0; sth < 8; sth++) {
					Serial.print(R_sen_bar.white(sth), DEC);
					Serial.print(" ");
				}
				Serial.println("]");
				#endif

				if (R_sen_bar.white(0)){
					//moved off of black line in front of robot
					on_white_check = true;
				} else if (!R_sen_bar.white(0) && on_white_check){
					if (F_sen_bar.white(0) && F_sen_bar.white(7)){
						//We have turned and are back on the black line
						STOP();
						break; //exit loop, we have turned
					}
				} else{
					//line sensors didn't work properly
					//use decoders
					if ((abs(current_l - old_l) >= quarter_turn) || 
					    (abs(current_r - old_r) >= quarter_turn)){
						STOP();
						break; //exit loop, we have turned
					}
				}
			}

			current_direction++;
			  
		}

		int get_odo_left() {
			return(odo_left.read());
		}

		int get_odo_right() {
			return(odo_right.read());
		}

};
