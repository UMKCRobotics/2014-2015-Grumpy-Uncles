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

    void attach(int n_pin, float n_const)
	:pin(n_pin), ir_const(n_const) {}
	
    
    float getDistance(){
      return ir_const*pow(analogRead(pin), -1.1);
    }
};



class worldSensor{
	private:
        	IRsensor front;
		IRsensor l_front;
        	IRsensor r_front;

        	//IRsensor l_rear;
        	//IRsensor r_rear;
	public:
		worldSensor(){
			
          		front.attach(A1,1621.5);
          		l_front.attach(A0,1621.5);
          		r_front.attach(A2,1611.5);

			//l_rear.attach(4,1611.5);
			//r_rear.attach(4,1611.5);
        	}
        
        
        	bool check_front_wall(){
			// 3 is fine for now, will find better value later
          		if (front.getDistance() < 3){
            			return true;
          		}
          
          		else{
            			return false;
          		}
        	}
        
       		bool check_right_wall(){
			// 3 is fine for now, will find better value later
          		if (r_front.getDistance() < 3){
            			return true;
          		}
          
          		else{
            			return false;
          		}
        	}
        
        	bool check_left_wall(){
			// 3 is fine for now, will find better value later
          		if (l_front.getDistance() < 3){
            			return true;
          		}
          
          		else{
            			return false;
          		}
        	}

		void detectOpenings(bool openings[3]){
			openings[0] = check_right_wall();
			openings[1] = check_front_wall();
			openings[2] = check_left_wall();
		}
        
    };
