#define DEFAULT_SPEED 150

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


    class motor{
      private:
        int forward_pin;
        int backward_pin;
      public:
        motor(int n_forward, int n_backward)
          :forward_pin(n_forward), backward_pin(n_backward) {}
        
        void M_Setup(){
          pinMode(forward_pin, OUTPUT);
          pinMode(backward_pin, OUTPUT);
        }
        
        void go_forward(int mvmt_speed) {
            analogWrite(backward_pin, LOW);
            //slow code here
            analogWrite(forward_pin, mvmt_speed);
        }
        
        void go_backward(int mvmt_speed) {
            analogWrite(forward_pin, LOW);
            //slow code here
            analogWrite(backward_pin, mvmt_speed);
        }
        
        void stop_mvmt(){
          analogWrite(forward_pin, LOW);
          analogWrite(backward_pin, LOW);
        }

    };
    
    class motorCommander{
      private:
         motor* MF_Left;
         motor* MB_Left;
         motor* MF_Right;
         motor* MB_Right;
         
         int turn_delay90 = 1000;
         enum Cardinal {NORTH, EAST, SOUTH, WEST };
         Cardinal current_direction = NORTH;
         
       public:
       

       
        motorCommander() {
          MF_Left = new motor(9, 10);
          MB_Left = new motor(3, 4);
          MF_Right = new motor(11, 3);
          MB_Right = new motor(1, 2);
        }
        
        ~ motorCommander(){
          delete MF_Left;
          delete MB_Left;
          delete MF_Right;
          delete MB_Right;
        }
         void mcSetup(){
         MF_Left->M_Setup();
         MB_Left->M_Setup();
         MF_Right->M_Setup();
         MB_Right->M_Setup();
        }
        void MOVE_FORWARD(int movement_speed = DEFAULT_SPEED) {
         MF_Left->go_forward(movement_speed);
         MB_Left->go_forward(movement_speed);
         MF_Right->go_forward(movement_speed);
         MB_Right->go_forward(movement_speed);
        }
        
        void MOVE_BACKWARD(int movement_speed = DEFAULT_SPEED){
         MF_Left->go_backward(movement_speed);
         MB_Left->go_backward(movement_speed);
         MF_Right->go_backward(movement_speed);
         MB_Right->go_backward(movement_speed);
        }
        
        Cardinal get_direction(){return current_direction;}
        
        void set_direction(Cardinal n_dir) {current_direction = n_dir;}
        
        
        void STOP(){
         MF_Left->stop_mvmt();
         MB_Left->stop_mvmt();
         MF_Right->stop_mvmt();
         MB_Right->stop_mvmt();
        }
         
         void TURN_RIGHT(int movement_speed = DEFAULT_SPEED){
           STOP();
           MF_Right->go_backward(movement_speed);
           MF_Left->go_forward(movement_speed);
           delay(turn_delay90); 
           STOP();
         }
         
         void TURN_LEFT(int movement_speed = DEFAULT_SPEED){
           STOP();
           MF_Left->go_backward(movement_speed);
           MF_Right->go_forward(movement_speed);
           delay(turn_delay90); 
           STOP();
         }
         
         void moveCardinal(Cardinal direction_input){
             int desired_direction = current_direction - direction_input;
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
                 MOVE_FORWARD();
                 break;
               case 3: case -1:
                 TURN_RIGHT();
                 MOVE_FORWARD();
                 break;
             }
             current_direction = direction_input;
         }

    };
 
