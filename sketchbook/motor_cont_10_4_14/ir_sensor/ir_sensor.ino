class IRsensor(){
  private:
    int pin;
    int threshold;
  public:
    IRsensor(int n_pin)
      :pin(n_pin) {}
      
    void set_thresh(int n_thresh) {threshold = n_thresh;}
    
    int THRESHOLD {return threshold;}
    
    void IRsensor_Setup(){
      pinMode(pin, INPUT);
    }
    
    int PIN() {return pin;}
};



class IRsensors(){
      /* this will be altered in the future.
      nothing in this class is set in stone
      and is subject to change based on the
      yet-to-be-acquired line sensor library. */
      private:
        IRsensor front;
          front.set_thresh(300);
        IRsensor l_front;
          l_front.set_thresh(190);
        IRsensor r_front;
          r_front.set_thresh(190);
        IRsensor l_rear;
          l_rear.set_thresh(300);
        IRsensor r_rear;
          r_rear.set_thresh(300);
      public:
        void IR_Setup{
          front.IRsensor_Setup();
          l_front.IRsensor_Setup();
          r_front.IRsensor_Setup();
          l_rear.IRsensor_Setup();
          r_rear.IRsensor_Setup();
        }
        
        bool check_front_wall(){
          if (front.THRESHOLD > analogRead(front.PIN())){
            return true;
          }
          
          else{
            return false;
          }
        }
        
        bool check_right_wall(){
          if (r_rear.THRESHOLD > analogRead(r_rear.PIN()) || r_front.THRESHOLD > analogRead(r_front.PIN())){
            return true;
          }
          
          else{
            return false;
          }
        }
        
        bool check_left_wall(){
          if (l_rear.THRESHOLD > analogRead(l_rear.PIN()) || l_front.THRESHOLD > analogRead(l_front.PIN())){
            return true;
          }
          
          else{
            return false;
          }
        }
        
    };
