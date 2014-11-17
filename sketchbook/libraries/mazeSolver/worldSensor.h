#include <Arduino.h>

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



class worldSensor{
	private:
        	IRsensor* front;
        	/*
		IRsensor* l_front;
        	IRsensor* r_front;
        	IRsensor* l_rear;
        	IRsensor* r_rear;
        	*/
	public:
		worldSensor(){
          		front = new IRsensor(0,1621.5);
			/*
          		l_front = new IRsensor(3,1621.5);
          		l_rear = new IRsensor(4,1611.5);
          		r_front = new IRsensor(5,1600.5);
	  		r_rear = new IRsensor(6,100);
			*/
        	}
        
        	~worldSensor(){
          		delete front;
	  		/*
	  		delete l_front;
          		delete r_front;
          		delete l_rear;
          		delete r_rear;
	  		*/
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
			//Balance the short and long sensor
			return true;
        	}
        
        	bool check_left_wall(){
			//Balance the short and long sensor
			return true;
        	}

		void detectOpenings(bool (&openings) [3]){
			openings[0] = check_right_wall();
			openings[1] = check_front_wall();
			openings[2] = check_left_wall();
		}
        
    };
