include "worldSensor.h"

class mazeSolver{
  private:
    worldSensor ws;
    bool openings [3];
  public:
    enum walls{R, F, L};
    enum Cardinal{NORTH,EAST,SOUTH,WEST};
    
    Cardinal computeNextmove()
    {
      ws.detectOpenings(openings);
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
 
