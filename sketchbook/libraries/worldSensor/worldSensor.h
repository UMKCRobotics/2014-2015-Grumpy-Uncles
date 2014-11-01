class worldSensor{
  private:
    IRsensors ir;  
  public:
    enum walls{F, L, R};
    void detectOpenings(bool (&p_openings)[3]){
      p_openings[F] = ir.check_front_wall();
      p_openings[L] = ir.check_left_wall();
      p_openings[R] = ir.check_right_wall();
    }
    
};


