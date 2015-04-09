
class mazeSolver{
  private:
    worldSensor ws;
    bool openings [3];
  public:
    enum walls{R, F, L};
    
    dir::Cardinal computeNextmove(char given) {
    	switch (given) {
    		case 'S':
    			return dir::SOUTH;
    			break;
    		case 'E':
    			return dir::EAST;
    			break;
    		case 'W':
    			return dir::WEST;
    			break;
    		case 'N':
    			return dir::NORTH;
    			break;
    	}
    }

    
    dir::Cardinal computeNextmove(dir::Cardinal current_direction)
    {
      ws.detectOpenings(openings);
      //Serial.print("R: ");
      //Serial.print(openings[R], DEC);
      //Serial.print(" F: ");
      //Serial.print(openings[F], DEC);
      //Serial.print(" L: ");
      //Serial.print(openings[L], DEC);
      //Serial.println(" \nNEXT: ");
      switch (current_direction){
        case dir::NORTH:
          if (openings[R])
            return dir::EAST;
          else if (openings[F])
            return dir::NORTH;
          else if (openings[L])
            return dir::WEST;
          else
            return dir::SOUTH;
            break;
         case dir::EAST:
           if (openings[R])
            return dir::SOUTH;
          else if (openings[F])
            return dir::EAST;
          else if (openings[L])
            return  dir::NORTH;
          else
            return dir::WEST;
            break;
        case dir::SOUTH:
          if (openings[R])
            return dir::WEST;
          else if (openings[F])
            return dir::SOUTH;
          else if (openings[L])
            return dir::EAST;
          else
            return dir::NORTH;
            break;
          case dir::WEST:
           if (openings[R])
            return dir::NORTH;
          else if (openings[F])
            return dir::WEST;
          else if (openings[L])
            return dir::SOUTH;
          else
            return dir::EAST;
            break;       
       
      }
}
    };
