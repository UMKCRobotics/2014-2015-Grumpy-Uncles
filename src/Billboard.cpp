class Billboard {
    private:
        #define START   0x0001
        #define S_0     0x093F      // zero
        #define S_1     0x0006      // one
        #define S_2     0x00DB      // two
        #define S_3     0x0084      // three
        #define S_4     0x00E7      // four
        #define S_5     0x00AD      // five
        #define S_6     0x00FD      // six
        #define S_7     0x2401      // seven
    // alternate 7
    //  #define S_7     0x0007      // seven
        #define S_8     0x00FF      // eight
        #define S_9     0x00E7      // nine
        #define S_O     0x003F      // 'O' (oh)
        #define S_K     0x0C70      // 'K'
        
        // is there a uint_16?
        uint_16 digit_tens;
        unit_16 digit_ones;
        uint_32 display_full;
        char display_pad;
        
    public:
        enum COLORS {
            RED = 1,
            YELLOW = 2,
            GREEN = 4
        };
        
        Billboard() {
            digit_tens = 0x00000000;
            digit_ones = 0x00000000;
            display_full = 0x00000000;
            display_pad = 0x00;
        }
        ~Billboard();
        
        void light(int color) {
            display_pad |= color;
        }
        
        void display(char cell_number) {
            // grab the ones place
            int ones = (cell_number % 10);
            // grab the tens place
            int tens = (cell_number / 10);
            
            // assuming MSB to right
            // need to verify this with tests
            
            
            if (cell_number == 79) {    // matches 'O' (oh)
                digit_tens = S_O;
                digit_ones = S_K;
            } else {
                switch (ones) {
                    case 0:
                        digit_ones = S_0;
                        break;
                    case 1:
                        digit_ones = S_1;
                        break;
                    case 2:
                        digit_ones = S_2;
                        break;
                    case 3:
                        digit_ones = S_3;
                        break;
                    case 4:
                        digit_ones = S_4;
                        break;
                    case 5:
                        digit_ones = S_5;
                        break;
                    case 6:
                        digit_ones = S_6;
                        break;
                    case 7:
                        digit_ones = S_7;
                        break;
                    case 8:
                        digit_ones = S_8;
                        break;
                    case 9:
                        digit_ones = S_9;
                        break;
                } // end (switch(ones))
                switch (tens) {
                    case 0:
                        digit_tens = S_0;
                        break;
                    case 1:
                        digit_tens = S_1;
                        break;
                    case 2:
                        digit_tens = S_2;
                        break;
                    case 3:
                        digit_tens = S_3;
                        break;
                    case 4:
                        digit_tens = S_4;
                        break;
                    case 5:
                        digit_tens = S_5;
                        break;
                    case 6:
                        digit_tens = S_6;
                        break;
                    case 7:
                        digit_tens = S_7;
                        break;
                    case 8:
                        digit_tens = S_8;
                        break;
                    case 9:
                        digit_tens = S_9;
                        break;
                }   // end (switch(tens))
            }   // end if
        //  end result:      sooooooooooooootttttttttttttt12pppp
            display_full |= 0x00000001;
            display_full << 14;
        //  after this: 0b00000000000000000100000000000000
            display_full |= digit_ones;
        //  after this: 0b000000000000000001oooooooooooooo
            display_full << 14;
        //  after this: 0b0001oooooooooooooo00000000000000
            display_full |= digit_tens;
        //  after this: 0b0001ooooooooooooootttttttttttttt

            xmit(display_full);
        //  for now, display_pad is empty (0's), but can be used to set
        //     decimal points and access the four other pins on the board.
        //     this will be the location of 
            xmit(display_pad);
        } // end (display(cell_number))
};
