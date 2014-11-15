class IRsensor{
  private:
    int pin;
    float ir_const;
  public:
    IRsensor(int n_pin, float n_const)
      :pin(n_pin), ir_const(n_const) {}
    
    float getConst() {return ir_const;}
    
    int getPin() {return pin;}
    
    float getDistance(){
      return ir_const*pow(analogRead(pin), -1.1);
    }
    
};



class IRsensors{
      /* this will be altered in the future.
      nothing in this class is set in stone
      and is subject to change based on the
      yet-to-be-acquired line sensor library. */
      private:
        IRsensor* front;
        IRsensor* left;
        IRsensor* right;
        /*IRsensor l_front;
        IRsensor r_front;
        IRsensor l_rear;
        IRsensor r_rear;
        */
      public:
        IRsensors(){
          front = new IRsensor(0,1621.5);
          left = new IRsensor(1,1611.5);
          right = new IRsensor(2,1600.5);
        }
        
        ~IRsensors(){
          delete front;
          delete left;
          delete right;
        }
        
        bool check_front_wall(){
          if (front->getDistance() < 3){
            return true;
          }
          
          else{
            return false;
          }
        }
        
        bool check_right_wall(){
          if (right->getDistance() < 3){
            return true;
          }
          
          else{
            return false;
          }
        }
        
        bool check_left_wall(){
          if (left->getDistance() < 3){
            return true;
          }
          
          else{
            return false;
          }
        }
        
    };
