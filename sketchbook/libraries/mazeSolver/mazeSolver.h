class mazeSolver{
  private:
    int current_pos = 0;
    int maze_end = 15; //4x4 maze 
    worldSensor ws;
    bool openings [3];
  public:
    enum walls{F, L, R};

    void nextMove() {
      // serial.readbyte
      // nexitDirection = current_cell - cell_num : { +1, -1, +7, -7 }
      // switch(next_direction) {
        // moveCardinal(NORTH)
      }
    
    dir::Cardinal computeNextmove(dir::Cardinal curr_direction)
    {
      ws.detectOpenings(openings);
      switch (curr_direction){
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
            return dir::NORTH;
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
 
