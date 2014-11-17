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
    //some LEDs
    
  public:
    Configurator(){
      
    }
    
    ~Configurator(){
      
    }
    
    void setRound()
    // read the round switch and store what round we're in,
      cur_round = digitalRead(round_switch_pin1);
      cur_round << 1;
      cur_round |= digitalRead(round_switch_pin2);
      
      switch(round_num) {
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
      
      switch(round_num) {
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
       //Then send the data.
       //use output_stream->write(BYTE)
    }
    
    int getThrottle(){
      return throttle;
    }
};
    
