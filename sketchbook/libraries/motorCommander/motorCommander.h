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
		LineSensors M_sen_bar;
		LineSensors B_sen_bar;
		const unsigned char F_sen_pins[6] = { 43, 45, 47, 42, 44, 46 };
		const unsigned char M_sen_pins[8] = { 14, 15, 16, 17, 18, 19, 34, 35 };
		const unsigned char B_sen_pins[2] = { 29, 28};

		void set_throttle() {
		//	throttle = map(analogRead(throttle_pin), 0, 1023, 0, 255);
		
			// this one attempts to slow down the acceleration
			//    ramp of the throttle pot
			throttle = map(analogRead(throttle_pin), 0, 1023, 30, 100);

			#ifdef GDEBUG
			Serial.print("MC :: set_throttle --> resetting to (");
			Serial.print(throttle, DEC);
			Serial.println(")");
			#endif
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
			quarter_turn = 740;
			one_foot = 1926;
		}

		void init(int F_thresh, int M_thresh, int B_thresh) {
			throttle_pin = A3;

			left_front.attach ( 7,  6);
			left_rear.attach  ( 4,  5);
			right_rear.attach ( 8, 10);
			right_front.attach(12, 11);

			F_sen_bar.init(F_sen_pins, 6, F_thresh);
			M_sen_bar.init(M_sen_pins, 8, M_thresh);
			B_sen_bar.init(B_sen_pins, 2, B_thresh);
		}

		void set_direction(dir::Cardinal new_direction) {
			current_direction = new_direction;
		}

		dir::Cardinal get_direction() {
			return current_direction;
		}

		short moveCardinal(dir::Cardinal move_to) {
			short desired_direction = current_direction - move_to;
			
			switch (desired_direction){
				case 0:
					MOVE_FORWARD();
					break;
				case -3: case 1:
				//	TURN_LEFT();
					TURN_("LEFT");
					B_sen_bar.poll_sensors();
					if((B_sen_bar.read_line() + 3000) < 0) {
						shimmy_right(true);
					} else {
						shimmy_left(true);
					}
					MOVE_FORWARD();
					break;
				case -1: case 3:
				//	TURN_RIGHT();
					TURN_("RIGHT");
					B_sen_bar.poll_sensors();
					if((B_sen_bar.read_line() + 3000) < 0) {
						shimmy_right(true);
					} else {
						shimmy_left(true);
					}
					MOVE_FORWARD();
					break;
				case -2: case 2:
				//	TURN_RIGHT();
				//	TURN_RIGHT();
					TURN_("RIGHT");
					TURN_("RIGHT");
					B_sen_bar.poll_sensors();
					if((B_sen_bar.read_line() + 3000) < 0) {
						shimmy_right(true);
					} else {
						shimmy_left(true);
					}
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

		void MOVE_FORWARD() {
			#ifdef GDEBUG
			Serial.println("MC :: FWD --> entering");
			#endif

			int odo_old = (odo_left.read() + odo_right.read())/2;
			int current;
			// make sure we're actually stopped.
			STOP();

			// reset the throttle value based on the pot.
			set_throttle();

			// this define needs a better home
			#define SCALING 0.04
			float FAST_SCALING = 1.00 + SCALING;
			float SLOW_SCALING = 1.00 - SCALING;
			int offset = 0;
			
			byte speed_l = throttle * FAST_SCALING;
			byte speed_r = throttle * SLOW_SCALING;

			#ifdef GDEBUG
			Serial.print("MC :: FWD --> throttle("); Serial.print(throttle, DEC);
			Serial.print(") offset("); Serial.print(offset, DEC);
			Serial.print(") speedl("); Serial.print(speed_l, DEC);
			Serial.print(") speedr("); Serial.print(speed_r, DEC);
			Serial.println(")");
			#endif

			// the initial push to begin moving forward.
			left_front.go_forward(speed_l);
			left_rear.go_forward(speed_l);
			right_front.go_forward(speed_r);
			right_rear.go_forward(speed_r);
			
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
				F_sen_bar.poll_sensors();
				M_sen_bar.poll_sensors();
				B_sen_bar.poll_sensors();
			} while(!M_sen_bar.white(0) || !M_sen_bar.white(7));
			#ifdef GDEBUG
			Serial.println("MC :: FWD --> off black. now to check");
			#endif

			short distance_moved;
			bool stop_condition = false;
			bool next_line_check = false;
			#define Kp 0.7
			#define Kd 3
			
			int error = 0;
			int lastError = 0;
			
			do {
				current = (odo_left.read() + odo_right.read())/2;
				distance_moved = abs(current-odo_old);
				#ifdef GDEBUG
//				Serial.print("Distance Moved: ");
//				Serial.print(distance_moved, DEC);
				#endif
				F_sen_bar.poll_sensors();
				M_sen_bar.poll_sensors();
				B_sen_bar.poll_sensors();
				error = M_sen_bar.read_line()/100;
				offset = Kp * error + Kd * (error-lastError);
//				offset = error;
				lastError = error;

				if (offset > 9 || offset < -9) {
					// extract the sign information from offset
					int off_sign = offset / offset;
					offset = 3 * sqrt(abs(offset));
					// return the sign to the result
					offset *= off_sign;
				}

				// PLUS for LEFT
				// MINUS for RIGHT
				speed_l = (throttle * FAST_SCALING) + offset;
				speed_r = (throttle * SLOW_SCALING) - offset;

				#ifdef GDEBUG
				Serial.print("MC :: FWD --> throttle("); Serial.print(throttle, DEC);
				Serial.print(") offset("); Serial.print(offset, DEC);
				Serial.print(") speedl("); Serial.print(speed_l, DEC);
				Serial.print(") speedr("); Serial.print(speed_r, DEC);
				Serial.print(") odo_left("); Serial.print(odo_left.read(), DEC);
				Serial.print(") odo_right("); Serial.print(odo_right.read(), DEC);
				Serial.print(") current("); Serial.print(current, DEC);
				Serial.println(")");
				#endif

				left_front.go_forward(speed_l);
				left_rear.go_forward(speed_l);
				right_front.go_forward(speed_r);
				right_rear.go_forward(speed_r);

				if (B_sen_bar.white(0) && B_sen_bar.white(1) && M_sen_bar.white(4)) {
					STOP();
					if (M_sen_bar.read_line() < 2000) {
						shimmy_left();
					} else if (M_sen_bar.read_line() > 2000) {
						shimmy_right();
					}
				}

				if (distance_moved > one_foot) {
					stop_condition = true;
				} else if (distance_moved < one_foot * .9) {
					continue;
				} else {
					if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
						stop_condition = true;
					}
				}
			} while (stop_condition == false);

			//Robot is back to black line and has moved forward a square
			#ifdef GDEBUG
			Serial.println("Back to black line");
			Serial.println("Stopped");
			#endif
			STOP();
		}

		void shimmy_left(bool turning = false) {
			const int waggle = 300;
			const int oldo_l = odo_left.read();
			const int oldo_r = odo_right.read();
			set_throttle();
			const int allowed = 4;

			bool stop_condition = false;
			do {
				B_sen_bar.poll_sensors();
				M_sen_bar.poll_sensors();
				if (abs(B_sen_bar.read_line() + 3000) < 10) {
					if (abs(M_sen_bar.read_line() < 100)) {
						if (turning) {
							if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
								stop_condition = true;
							}
						} else {
							stop_condition = true;
						}
					}
				}

				right_front.go_forward(throttle);
				while (abs(odo_right.read() - oldo_l) < waggle);
				right_front.stop();
				if (abs(B_sen_bar.read_line() + 3000) < 10) {
					if (abs(M_sen_bar.read_line() < 100)) {
						if (turning) {
							if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
								stop_condition = true;
							}
						} else {
							stop_condition = true;
						}
					}
				}

				left_front.go_forward(throttle);
				while (abs(odo_left.read() - oldo_r) < waggle);
				left_front.stop();
				if (abs(B_sen_bar.read_line() + 3000) < 10) {
					if (abs(M_sen_bar.read_line() < 100)) {
						if (turning) {
							if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
								stop_condition = true;
							}
						} else {
							stop_condition = true;
						}
					}
				}

				right_front.go_reverse(throttle);
				while (odo_right.read() > oldo_l);
				right_front.stop();
				if (abs(B_sen_bar.read_line() + 3000) < 10) {
					if (abs(M_sen_bar.read_line() < 100)) {
						if (turning) {
							if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
								stop_condition = true;
							}
						} else {
							stop_condition = true;
						}
					}
				}

				left_front.go_reverse(throttle);
				while (odo_left.read() > oldo_r);
				left_front.stop();
			} while(stop_condition == false);
		}

		void shimmy_right(bool turning = false) {
			const int waggle = 300;
			const int oldo_l = odo_left.read();
			const int oldo_r = odo_right.read();
			set_throttle();
			const int allowed = 4;

			bool stop_condition = false;
			do {
				B_sen_bar.poll_sensors();
				M_sen_bar.poll_sensors();
				if (abs(B_sen_bar.read_line() + 3000) < 10) {
					if (abs(M_sen_bar.read_line() < 100)) {
						if (turning) {
							if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
								stop_condition = true;
							}
						} else {
							stop_condition = true;
						}
					}
				}

				left_front.go_forward(throttle);
				while (abs(odo_left.read() - oldo_l) < waggle);
				left_front.stop();
				if (abs(B_sen_bar.read_line() + 3000) < 10) {
					if (abs(M_sen_bar.read_line() < 100)) {
						if (turning) {
							if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
								stop_condition = true;
							}
						} else {
							stop_condition = true;
						}
					}
				}

				right_front.go_forward(throttle);
				while (abs(odo_right.read() - oldo_r) < waggle);
				right_front.stop();
				if (abs(B_sen_bar.read_line() + 3000) < 10) {
					if (abs(M_sen_bar.read_line() < 100)) {
						if (turning) {
							if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
								stop_condition = true;
							}
						} else {
							stop_condition = true;
						}
					}
				}

				left_front.go_reverse(throttle);
				while (odo_left.read() > oldo_l);
				left_front.stop();
				if (abs(B_sen_bar.read_line() + 3000) < 10) {
					if (abs(M_sen_bar.read_line() < 100)) {
						if (turning) {
							if (!M_sen_bar.white(0) && !M_sen_bar.white(7)) {
								stop_condition = true;
							}
						} else {
							stop_condition = true;
						}
					}
				}

				right_front.go_reverse(throttle);
				while (odo_right.read() > oldo_r);
				right_front.stop();
			} while(stop_condition == false);
		}

		void TURN_(const char* dir) {
			//This using encoder data and a turn constant to properly turn
			bool stop_l = false;
			bool stop_r = false;
			int32_t old_l = odo_left.read();
			int32_t old_r = odo_right.read();
			int32_t current_l;
			int32_t current_r;
			byte speed_rear;

			set_throttle();
			// yes, this will demote the actual answer to an
			//    integer, but that's okay. we can afford to
			//    lose some precision on this.
			speed_rear = rear_motor_ratio * throttle;

			if (dir == "LEFT" || dir == "left") {
				left_front.go_reverse(throttle);
				left_rear.go_reverse(speed_rear);
				right_front.go_forward(throttle);
				right_rear.go_forward(speed_rear);
			} else {
				left_front.go_forward(throttle);
				left_rear.go_forward(speed_rear);
				right_front.go_reverse(throttle);
				right_rear.go_reverse(speed_rear);
			}
			
			#ifdef GDEBUG
			Serial.print("MC :: TURN_ --> throttle: ");
			Serial.print(throttle, DEC);
			Serial.print(", scaled: ");
			Serial.print(speed_rear, DEC);
			Serial.println();
			#endif

			bool stop_condition = false;
			//wait to get off of black line
			while (!B_sen_bar.white(0) || !B_sen_bar.white(1)){
				B_sen_bar.poll_sensors();
			}//Rear sensors are off black line
			do {
				current_l = odo_left.read();
				current_r = odo_right.read();

				if ((stop_l == false) &&
					(abs(current_l - old_l) >= quarter_turn)) {
					left_front.stop();
					left_rear.stop();
					stop_l = true;
				}

				if ((stop_r == false) &&
					(abs(current_r - old_r) >= quarter_turn)) {
					right_front.stop();
					right_rear.stop();
					stop_r = true;
				}

				if (stop_r == true && stop_l == true) {
					#ifdef GDEBUG
					Serial.println("MC :: TURN_ --> reached BACKUP stop condition");
					#endif
					stop_condition = true;
				}
			} while (stop_condition == false);

			if (dir == "LEFT" || dir == "left") {
				current_direction--;
			} else {
				current_direction++;
			}

			STOP();
			#ifdef GDEBUG
			Serial.println("MC :: TL --> STOPPED");
			#endif
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

			bool stop_condition = false;
			//wait to get off of black line
			while (!B_sen_bar.white(0) || !B_sen_bar.white(1)){
				B_sen_bar.poll_sensors();
			}//Rear sensors are off black line
			do {
				current_l = odo_left.read();
				current_r = odo_right.read();
				B_sen_bar.poll_sensors();

				if ((stop_l == false) &&
					(abs(current_l - old_l) >= quarter_turn)) {
					left_front.stop();
					left_rear.stop();
					stop_l = true;
				}

				if ((stop_r == false) &&
					(abs(current_r - old_r) >= quarter_turn)) {
					right_front.stop();
					right_rear.stop();
					stop_r = true;
				}

				if (stop_r == true && stop_l == true) {
					stop_condition = true;
				}
			} while (stop_condition == false);
			STOP();
			#ifdef GDEBUG
			Serial.println("MC :: TL --> STOPPED");
			#endif
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

			bool stop_condition = false;
			//wait to get off of black line
			while (!B_sen_bar.white(0) && !B_sen_bar.white(1)
			   && (!M_sen_bar.white(0) || !M_sen_bar.white(7))){
			    F_sen_bar.poll_sensors();
			    M_sen_bar.poll_sensors();
				B_sen_bar.poll_sensors();
			}//Rear sensors are off black line
			do {
				current_l = odo_left.read();
				current_r = odo_right.read();

				if ((stop_l == false) &&
					(abs(current_l - old_l) >= quarter_turn)) {
					left_front.stop();
					left_rear.stop();
					stop_l = true;
				}

				if ((stop_r == false) &&
					(abs(current_r - old_r) >= quarter_turn)) {
					right_front.stop();
					right_rear.stop();
					stop_r = true;
				}

				if (stop_r == true && stop_l == true) {
					stop_condition = true;
                }
			} while (stop_condition == false);
			STOP();
			#ifdef GDEBUG
			Serial.println("MC :: TL --> STOPPED");
			#endif
			current_direction++;
		}

		int get_odo_left() {
			return(odo_left.read());
		}

		int get_odo_right() {
			return(odo_right.read());
		}

		void poke() {
			set_throttle();
			F_sen_bar.poll_sensors();
			M_sen_bar.poll_sensors();
			B_sen_bar.poll_sensors();
		}

		void dump() {
			Serial.println("MC :: dump -->");
			Serial.print("\tthrottle: "); Serial.println(throttle, DEC);
			Serial.print("\tdirection: "); Serial.println(current_direction);

			Serial.print("\tF_SEN_BAR: [");
			for (int sth = 0; sth < 8; sth++) {
				Serial.print(F_sen_bar.value(sth), DEC);
				Serial.print(" ");
			}   Serial.println("]");
			Serial.print("\tM_SEN_BAR: [");
			for (int sth = 0; sth < 8; sth++) {
				Serial.print(M_sen_bar.value(sth), DEC);
				Serial.print(" ");
			}   Serial.println("]");
			Serial.print("\tB_SEN_BAR: [");
			for (int sth = 0; sth < 8; sth++) {
				Serial.print(B_sen_bar.value(sth), DEC);
				Serial.print(" ");
			}   Serial.println("]");
		}
};
