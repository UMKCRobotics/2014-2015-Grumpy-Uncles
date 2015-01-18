#include <HardwareSerial.h>
#include <Arduino.h>

class Configurator{
  private:
    int round_switch_pin1;
    int round_switch_pin2;
    int part_switch_pin1;
    int part_switch_pin2;
    int throttle_pin;
    int throttle;
    int cur_round;
    int cur_part;
    
	char OP_MOVE;
	char OP_OK;
	char OP_SYN;
	char OP_ACK;

    
  public:
    Configurator() {
		throttle_pin = 0;
		part_switch_pin1 = 0;
		part_switch_pin2 = 0;
		round_switch_pin1 = 0;
		round_switch_pin2 = 0;

		OP_MOVE = 0xE0;
		OP_OK   = 0xE1;
		OP_SYN  = 0xE2;
		OP_ACK  = 0xE3;
    }
    
    ~Configurator(){
      
    }
    
    void setRound() {
    // read the round switch and store what round we're in,
      cur_round = digitalRead(round_switch_pin1);
      cur_round << 1;
      cur_round |= digitalRead(round_switch_pin2);
      
      switch(cur_round) {
          // 00, 01, 10, 11
          case 00:
            cur_round = 1;
            break;
          case 01:
            cur_round = 2;
            break;
          case 10:
            cur_round = 3;
            break;
       }
    }
    
    void setPart(){
      cur_part = digitalRead(part_switch_pin1);
      cur_part << 1;
      cur_part |= digitalRead(part_switch_pin2);
      
      switch(cur_part) {
          // 00, 01, 10, 11
          case 11: case 00:
            cur_part = 1;
            break;
          case 01: case 10:
            cur_part = 2;
            break;
       }
    }
    
    void setThrottle(){
      throttle = analogRead(throttle_pin);
      //convert to a useable value
    }
    
    void sendData(HardwareSerial* output_stream){
       //Wait on sync byte that says we are ready to transmit.
       char synack = 0x00;
       do {
           synack = output_stream->read();
       } while(synack != OP_SYN);
       output_stream->write(OP_ACK);

       //Then send the data.
       //use output_stream->write(BYTE)
       output_stream->write(cur_round);
       output_stream->write(cur_part);
    }
    
    int getThrottle(){
      return throttle;
    }
};
