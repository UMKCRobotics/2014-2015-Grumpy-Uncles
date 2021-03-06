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

		int last_std_dev = -1;
		int lastError = 0;

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
//			throttle = map(analogRead(throttle_pin), 0, 1023, 30, 100);
//			quarter_turn = map(throttle, 20, 60, 820, 740);

			// hard code value for sanity. this will now ignore the pot.
			throttle = 54;
			#ifdef GDEBUG
			Serial.print("MC :: set_throttle --> resetting to (");
			Serial.print(throttle, DEC);
			Serial.print(") quarter_turn(");
			Serial.print(quarter_turn, DEC);
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
//			rear_motor_ratio = .70909;
			rear_motor_ratio = .69;
			// through experimentation, we need to adjust by the
			//    difference (1 - rear_motor_ratio) to get the
			//    adjusted speed for the wheels.
			//rear_motor_ratio = .29191;


			// left side is backwards owing to pinning.
			odo_left.attach(27, 26);
			odo_right.attach(24, 25);

			// a full revolution of the encoder is 1200 ticks.
			quarter_revolution = 300;
			// a quarter turn (based on math) 820
			quarter_turn = 750;
		//	one_foot = 1926;
			one_foot = 2000;
		}

		void init(int F_thresh, int M_thresh, int B_thresh) {
			throttle_pin = A3;

			left_front.attach ( 7,  6);
			left_rear.attach  ( 4,  5);
			right_front.attach(12, 11);
			right_rear.attach ( 8, 10);

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
					last_std_dev = -1;
					lastError = 0;
					MOVE_FORWARD();
					break;
				case -1: case 3:
				//	TURN_RIGHT();
					TURN_("RIGHT");
					last_std_dev = -1;
					lastError = 0;
					MOVE_FORWARD();
					break;
				case -2: case 2:
				//	TURN_RIGHT();
				//	TURN_RIGHT();
					TURN_("RIGHT");
					last_std_dev = -1;
					lastError = 0;
					TURN_("RIGHT");
					last_std_dev = -1;
					lastError = 0;
					MOVE_FORWARD();
					break;
			}

			return(desired_direction);
		}

		void STOP() {
			left_front.stop();
			right_front.stop();
			left_rear.stop();
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
			#define SCALING 0.00
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
			right_front.go_forward(speed_r);
			left_rear.go_forward(speed_l);
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
			#ifdef GDEBUG
			Serial.println("MC :: FWD --> move off black");
			#endif
			do {
				F_sen_bar.poll_sensors();
				M_sen_bar.poll_sensors();
				B_sen_bar.poll_sensors();
				current = (odo_left.read() + odo_right.read())/2;
//				#ifdef GDEBUG
//				Serial.print("MC :: FWD --> wait, 0(");
//				Serial.print(!M_sen_bar.white(0), DEC);
//				Serial.print(") 7(");
//				Serial.print(!M_sen_bar.white(7), DEC);
//				Serial.println(")");
//				#endif
//			} while(!M_sen_bar.white(0) || !M_sen_bar.white(7));
			} while (abs(current - odo_old) < quarter_revolution);
			#ifdef GDEBUG
			Serial.println("MC :: FWD --> off black. now to check");
			#endif

			short distance_moved;
			bool stop_condition = false;
			bool next_line_check = false;
			#define Kp 0.3
			#define Kd 6
			
			int error = 0;
			int error_net[5] = { 0 };
			int round_std_dev = 0;
			bool approach = false;
			
			do {
				F_sen_bar.poll_sensors();
				M_sen_bar.poll_sensors();
				B_sen_bar.poll_sensors();
				current = (odo_left.read() + odo_right.read())/2;
				distance_moved = abs(current-odo_old);

				#ifdef GDEBUG
				Serial.print("MC :: FWD --> TOP -- [ ");
				for (int sth = 0; sth < 8; sth++) {
					Serial.print(M_sen_bar.value(sth), DEC);
					Serial.print(" ");
				}
				Serial.println("]");
				#endif

				for (int rth = 0; rth < 5; rth++) {
					error_net[rth] = M_sen_bar.read_line(3500);
				}

				#ifdef GDEBUG
				Serial.print("MC :: FWD --> five reads: [ ");
				for (int rth = 0; rth < 5; rth++) {
					Serial.print(error_net[rth], DEC);
					Serial.print(" ");
				}
				Serial.println("]");
				#endif

				bool flag = true;
				for (int ith = 1; (ith <= 5) && flag; ith++) {
					flag = false;
					for (int jth = 0; jth < 4; jth++) {
						if (error_net[jth+1] > error_net[jth]) {
							int tmp = error_net[jth];
							error_net[jth] = error_net[jth+1];
							error_net[jth+1] = tmp;
							flag = true;
						}
					}
				}
				error = error_net[2] / 100;

				#ifdef GDEBUG
				Serial.print("MC :: FWD --> SORTED reads: [ ");
				for (int rth = 0; rth < 5; rth++) {
					Serial.print(error_net[rth], DEC);
					Serial.print(" ");
				}
				Serial.print("] --> ");
				Serial.println(error, DEC);
				#endif

//				error  = M_sen_bar.read_line(3500) / 100;
//				error -= B_sen_bar.read_line(500) / 10;
				offset = Kp * error + Kd * (error-lastError);
				lastError = error;
				if (round_std_dev > 0) {
					last_std_dev = round_std_dev;
				}

				if (offset > 9 || offset < -9) {
					// extract the sign information from offset
					bool negative = (offset < 0 ? true : false);
					offset = 3 * sqrt(abs(offset));
					// return the sign to the result
					if (negative) {
						offset *= -1;
					}
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
				right_front.go_forward(speed_r);
				left_rear.go_forward(speed_l);
				right_rear.go_forward(speed_r);

				if (distance_moved > one_foot) {
					#ifdef GDEBUG
					Serial.println("MC :: FWD --> exceeded allowed travel");
					#endif
					stop_condition = true;
				} else if (distance_moved < one_foot * .8) {
					continue;
				} else {
					if (approach == false) {
						throttle = throttle / 2;
						approach = true;
					}
					if (!F_sen_bar.white(0) && !F_sen_bar.white(5)) {
						#ifdef GDEBUG
						Serial.println("MC :: FWD --> Back to black line");
						#endif
						stop_condition = true;
					} else {
						#ifdef GDEBUG
						Serial.print("MC :: FWD --> stop? F.white(0)=");
						Serial.print(!F_sen_bar.white(0), DEC);
						Serial.print(", F.white(7)=");
						Serial.println(!F_sen_bar.white(5), DEC);
						#endif
					}
				}
			} while (stop_condition == false);

			//Robot is back to black line and has moved forward a square
			#ifdef GDEBUG
			Serial.println("Stopped");
			#endif
			STOP();
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

			int turn_stop = (dir == "LEFT") ? 750 : 760;

			set_throttle();
			#define SCALING 0.06
			float FAST_SCALING = 1.00 + SCALING;
			float SLOW_SCALING = 1.00 - SCALING;

			// yes, this will demote the actual answer to an
			//    integer, but that's okay. we can afford to
			//    lose some precision on this.
			speed_rear = rear_motor_ratio * throttle;

			if (dir == "LEFT" || dir == "left") {
				left_front.go_reverse(throttle);
				right_front.go_forward(throttle);
				left_rear.go_reverse(speed_rear);
				right_rear.go_forward(speed_rear);
			} else {
				left_front.go_forward(throttle);
				right_front.go_reverse(throttle);
				left_rear.go_forward(speed_rear);
				right_rear.go_reverse(speed_rear);
			}
			
			#ifdef GDEBUG
			Serial.print("MC :: TURN_");
			Serial.print(dir);
			Serial.print(" --> throttle: ");
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
					(abs(current_l - old_l) >= turn_stop)) {
					left_front.stop();
					left_rear.stop();
					stop_l = true;
				}

				if ((stop_r == false) &&
					(abs(current_r - old_r) >= turn_stop)) {
					right_front.stop();
					right_rear.stop();
					stop_r = true;
				}

				if (stop_r == true && stop_l == true) {
					#ifdef GDEBUG
					Serial.print("MC :: TURN_");
					Serial.print(dir);
					Serial.println(" --> reached REVOLUTION stop condition");
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
			Serial.print("MC :: TURN_");
			Serial.print(dir);
			Serial.println(" --> STOPPED");
			#endif
//			delay(25);
		}

		int get_odo_left() {
			return(odo_left.read());
		}

		int get_odo_right() {
			return(odo_right.read());
		}

		void poke(int motor = 0) {
			set_throttle();
			F_sen_bar.poll_sensors();
			M_sen_bar.poll_sensors();
			B_sen_bar.poll_sensors();

			switch (motor) {
				case 1:
					left_front.go_forward(throttle);
					delay(200);
					left_front.stop();
					break;
				case 2:
					left_rear.go_forward(throttle);
					delay(200);
					left_rear.stop();
					break;
				case 3:
					right_front.go_forward(throttle);
					delay(200);
					right_front.stop();
					break;
				case 4:
					right_rear.go_forward(throttle);
					delay(200);
					right_rear.stop();
					break;
			}
		}

		void spinlock() {
			set_throttle();

			left_front.go_forward(throttle / 2);
			right_front.go_reverse(throttle / 2);
			left_rear.go_forward(throttle / 2);
			right_rear.go_reverse(throttle / 2);
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
