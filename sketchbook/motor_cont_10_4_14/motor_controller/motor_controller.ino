enum class Cardinal {NORTH, EAST, SOUTH, WEST, END_OF_LIST };

/*
// Special behavior for ++Cardinal
Cardinal& operator++(Cardinal &c ) {
  c = static_cast<Cardinal>( static_cast<int>(c) + 1 );
  if ( c == Cardinal::END_OF_LIST )
    c = Cardinal::NORTH;
  return c;
}

// Special behavior for Cardinal++
Cardinal operator++(Cardinal &c, int ) {
  Cardinal result = c;
  ++c;
  return result;
}
*/


    class Motor_Cont{
      private:
        int foward_pin;
        int backward_pin;
      public:
        Motor_Cont(int n_foward, int n_backward){
          foward_pin = n_foward;
          backward_pin = n_backward;
        }
        
        void MC_Setup(){
          pinMode(foward_pin, OUTPUT);
          pinMode(backward_pin, OUTPUT);
        }
        
        go_foward(int mvmt_speed) {
            analogWrite(backward_pin, LOW);
            //slow code here
            analogWrite(foward_pin, mvmt_speed);
        }
        
        go_backward(int mvmt_speed) {
            analogWrite(foward_pin, LOW);
            //slow code here
            analogWrite(backward_pin, mvmt_speed);
        }
        
        stop_mvmt(){
          analogWrite(foward_pin, LOW);
          analogWrite(backward_pin, LOW);
        }

    };
    
    class Drive_Sys{
      private:
         int default_speed = 150;
         Motor_Cont MF_Left(1, 2);
         Motor_Cont MB_Left(3, 4);
         Motor_Cont MF_Right(5, 6);
         Motor_Cont MB_Right(7, 8);
         
         int turn_delay90 = 1000;
    
         Cardinal curr_direction = NORTH;
         
       public:
        void DS_Setup(){
         MF_Left.MC_Setup();
         MB_Left.MC_Setup();
         MF_Right.MC_Setup();
         MB_Right.MC_Setup();
        }
        void MOVE_FORWARD(int movement_speed = default_speed) {
         MF_Left.go_foward(movement_speed);
         MB_Left.go_foward(movement_speed);
         MF_Right.go_foward(movement_speed);
         MB_Right.go_foward(movement_speed);
        }
        
        void MOVE_BACKWARD(int movement_speed = default_speed){
         MF_Left.go_backward(movement_speed);
         MB_Left.go_backward(movement_speed);
         MF_Right.go_backward(movement_speed);
         MB_Right.go_backward(movement_speed);
        }
        
        Cardinal get_direction(){return curr_direction;}
        
        void set_direction(Cardinal n_dirr) {curr_direction = n_dirr;}
        
        
        void STOP(){
         MF_Left.stop_mvmt();
         MR_Left.stop_mvmt();
         MF_Right.stop_mvmt();
         MB_Right.stop_mvmt();
        }
         
         void TURN_RIGHT(){
           this.STOP();
           MF_Right.go_backward();
           MF_Left.go_foward();
           delay(turn_delay90); 
           this.STOP();
         }
         
         void TURN_LEFT(){
           this.STOP();
           MF_Left.go_backward();
           MF_Right.go_foward();
           delay(turn_delay90); 
           this.STOP();
         }
         
         void MOVE_CARDINAL(Cardinal direction_input){
             int desired_direction = curr_direction - direction_input;
             switch (desired_direction){
               case 0:
                 MOVE_FORWARD();
                 break;
               case 1: case -3:
                 TURN_LEFT();
                 MOVE_FORWARD();
                 break;
               case 2: case -2:
                 TURN_RIGHT();
                 TURN_RIGHT();
                 MOVE_FOWARD();
                 break;
               case 3: case -1:
                 TURN_RIGHT();
                 MOVE_FOWARD();
                 break;
             }
             curr_direction = direction_input;
         }

    };
    
    /* will eventually need an input class that
       reads commands from the serial line to
       control the motor drive_system */
 
