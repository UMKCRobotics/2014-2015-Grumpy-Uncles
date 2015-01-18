//#include "/home/umkc/robot/arduino/2014-2015-Grumpy-Uncles/sketchbook/libraries/Cardinal/Cardinal.h"
#include "Cardinal.h"

class mazeSolver{
  private:
    worldSensor ws;
    bool openings [3];
  public:
    enum walls{R, F, L};
    
    dir::Cardinal computeNextmove(dir::Cardinal current_direction)
    {
      ws.detectOpenings(openings);
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
         case dir::EAST:
           if (openings[R])
            return dir::SOUTH;
          else if (openings[F])
            return dir::EAST;
          else if (openings[L])
            return  dir::NORTH;
          else
            return dir::EAST;
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
 
