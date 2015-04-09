#include <HardwareSerial.h>
#include <Arduino.h>

class Configurator{
  private:
    byte round_switch_pin1;
    byte round_switch_pin2;
    byte round_switch_feed;
    byte part_switch_pin1;
    byte part_switch_pin2;
    byte part_switch_feed;
    byte throttle_pin;
    byte throttle;
    int cur_round;
    int cur_part;
    
  public:
	static const char OP_MOVE  = 'm';
	static const char OP_OK    = 'o';
	static const char OP_SYN   = 's';
	static const char OP_ACK   = 'a';
	static const char OP_RESET = 'r';
    
    Configurator() {
		throttle_pin = A3;
		round_switch_pin1 = 37;
		round_switch_feed = 39;
		round_switch_pin2 = 41;
		part_switch_pin1 = 36;
		part_switch_feed = 38;
		part_switch_pin2 = 40;

		throttle = 0;
		cur_round = 0;
		cur_part = 0;
	}

	void initialize() {
		pinMode(round_switch_pin1, INPUT);
		pinMode(round_switch_pin2, INPUT);
		digitalWrite(round_switch_pin1, HIGH);
		digitalWrite(round_switch_pin2, HIGH);
		pinMode(round_switch_feed, OUTPUT);
		digitalWrite(round_switch_feed, LOW);

		pinMode(part_switch_pin1, INPUT);
		pinMode(part_switch_pin2, INPUT);
		digitalWrite(part_switch_pin1, HIGH);
		digitalWrite(part_switch_pin2, HIGH);
		pinMode(part_switch_feed, OUTPUT);
		digitalWrite(part_switch_feed, LOW);
    }
    
    ~Configurator(){
      
    }
    
    void setRound() {
    // read the round switch and store what round we're in,
    // this will:
    //    - read one switch pin as 1 or 0,
    //    - shift that bit right one place
    //    - bitwise-or the result with the second switch pin.
    //
    // possible values (from testing) are:
    //
    // b00000001 - switch in REAR position; read as round one
    // b00000011 - switch in MIDDLE position; read as round two
    // b00000010 - switch in FORWARD position; read as round three
      if (digitalRead(round_switch_pin1) == 1) {
      	if (digitalRead(round_switch_pin2) == 0) {
      		cur_round = 3;
      	} else {
      		cur_round = 2;
      	}
      } else {
      	cur_round = 1;
      }
    }
    
    void setPart(){
    // read the round switch and store what part we're in,
    // this will:
    //    - read one switch pin as 1 or 0,
    //    - shift that bit right one place
    //    - bitwise-or the result with the second switch pin.
    //
    // possible values (from testing) are:
    //
    // b00000001 - switch in REAR position; read as part one
    // b00000010 - switch in FORWARD position; read as part two
    	if (digitalRead(part_switch_pin1) == 0) {
    		cur_part = 1;
    	} else {
    		cur_part = 2;
    	}
    }
    
    void setThrottle(){
      //throttle = analogRead(throttle_pin);
      // convert to a useable value

      // since we're passing it up inside a char,
      //    reasonable values are between 0 and 255
      //
      // need to determine what the maximum value
      //    of the potentiometer is.
      throttle = map(analogRead(throttle_pin), 0, 1024, 0, 255);
    }
    
    void sendConfig(HardwareSerial* output_stream){
       // decdided not to wait on a sync. this data should
       //    sit in the buffer until called upon.
       //
       // a whole other way to do this is to have the upper
       //    layer use the arduinoInterface to ask the arduino
       //    what it wants. 
       //
       //    udoo          arduino
       //    
       //    OP_ROUND -------->
       //        <--------  round #
       //    OP_PART  -------->
       //        <--------  part #
       //
       //Then send the data.
       //use output_stream->write(BYTE)
//       output_stream->write((byte)cur_round);
//       output_stream->write((byte)cur_part);
       output_stream->print(cur_round, DEC);
       output_stream->print(cur_part, DEC);
    }
    
    byte getThrottle(){
      return throttle;
    }

    int getPart() {
    	return cur_part;
    }

    int getRound() {
    	return cur_round;
    }

    bool lobatt() {
        float level = analogRead(A6);
        // Darren's magic number
        level *= 0.010877;
        // Darren recommends:
        //    warning at 6.6
        //    shutoff at 6.4
        return  (level < 6.6);
    }
};
