#include <Arduino.h>

class IRsensor{
  private:
    int pin;
    float ir_const;
  public:
    IRsensor() {}
    IRsensor(int n_pin, float n_const)
      :pin(n_pin), ir_const(n_const) {}
    
    float getConst() {return ir_const;}
    
    int getPin() {return pin;}

    void attach(int n_pin, float n_const)
	{
	   pin = n_pin; 
           ir_const = n_const;
	}
	
    
    float getDistance(){
      float average = (analogRead(pin)+analogRead(pin)+analogRead(pin)+analogRead(pin)+analogRead(pin))/5;
	if (average == (1.0/0.0))
		return 1000;
      return ir_const*pow(average, -1.1);
    }
};



class worldSensor{
	private:
        	IRsensor front;
		IRsensor l_front;
        	IRsensor r_front;

			float long_sane_wall_distance;
			float short_sane_wall_distance;
        	IRsensor l_rear;
        	IRsensor r_rear;
	public:
		worldSensor(){
			long_sane_wall_distance = 10.0;
			short_sane_wall_distance = 5.0;

          		front.attach(A1,1610.5);
          		l_front.attach(A5,1550.5);
          		r_front.attach(A4,1550.5);

			l_rear.attach(A2,1631.5);
			r_rear.attach(A0,1631.5);
        	}

        	bool check_front_wall(){
        		//Serial.print(front.getDistance(), DEC);
        		//Serial.print("\n");

          		if (front.getDistance() > long_sane_wall_distance){
            			return true;
          		}
          
          		else{
            			return false;
          		}
        	}
        
       		bool check_right_wall(){


			float internal_front = r_front.getDistance();
			float internal_rear = r_rear.getDistance();

			//Serial.print(" \n");
			//Serial.print("RIGHT: ");
        		//Serial.print(internal_front, DEC);
        		//Serial.print("  ");
			//Serial.print(internal_rear, DEC);
			//Serial.print(" \n");

          		if (internal_front > long_sane_wall_distance){
				if  (internal_rear > short_sane_wall_distance)
            				return true;
          		}

			else if (internal_rear > short_sane_wall_distance)
				return true;
          
          		else{
            			return false;
          		}
        	}
        
        	bool check_left_wall(){
			//Serial.print("FRONT: ");
        		//Serial.print(l_front.getDistance(), DEC);
        		//Serial.print("  ");
			//Serial.print(r_front.getDistance(), DEC);

			float internal_front = l_front.getDistance();
			float internal_rear = l_rear.getDistance();

          		if (internal_front > long_sane_wall_distance){
				//if (internal_rear > short_sane_wall_distance)
            				return true;
          		}
			
			//this is bad, change to rear once IR sensor is placed correctly
//			else if (internal_front > short_sane_wall_distance)
				//return true;

          
          		else{
            			return false;
          		}
        	}

		void detectOpenings(bool openings[3]){
			openings[0] = check_right_wall();
			openings[1] = check_front_wall();
			openings[2] = check_left_wall();
		//	Serial.println();
		}
        
    };
