      
    Drive_Sys drive_system();
    IRsensors ir_sensors();
    LineSensors line_sensors();
    
    int current_pos = 0;
    int maze_end = 15; //4x4 maze 
    
    bool openings [3];
    
    Cardinal computeNextmove(Array<bool> &openings)
    {
      switch (curr_dirrection){
        case NORTH:
          if (openings[R])
            return EAST;
          else if (openings[F]
            return NORTH;
          else if (openings[L]
            return WEST;
          else
            return SOUTH;
            break;
        case SOUTH:
          if (openings[R])
            return WEST;
          else if (openings[F]
            return SOUTH;
          else if (openings[L]
            return EAST;
          else
            return NORTH;
            break;
         case EAST:
           if (openings[R])
            return SOUTH
          else if (openings[F]
            return EAST;
          else if (openings[L]
            return  NORTH;
          else
            return EAST;
            break;
          case WEST:
           if (openings[R])
            return NORTH;
          else if (openings[F]
            return WEST;
          else if (openings[L]
            return SOUTH;
          else
            return EAST;
            break;
      }
    }
    void setup() {
      drive_system.DS_Setup();
      ir_sensors.IR_Setup();
      line_sensors.LS_Setup();

      Serial.begin(9600);
    }

    void loop() {
     
      
      /* check the serial line for input;
         if input {
           process command
           flush input line
         }
         
         switch (command) {
           case motors:
             send command to motors
           case sensor:
             read state of sensors
             return values to the serial line?
           case display:
             write supplied data to the segment displays
           default
             do nothing
         }
         
         (flush input and output again)
      */
    }

