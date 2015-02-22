#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>     // uint16_t
#include <string>

//#include "SPI.h"       // spi device with segment display

#define freshen(c) {                    \
    for (int cth = 0; cth < 5; cth++)   \
        c[cth] = 0x00;                  \
    c[0]  = (1 << 6);                   \
}

#define ones(c, n) {                    \
    c[0] &= 0xC0;                       \
    c[1] &= 0x00;                       \
    c[0] |= ((n & 0x3F00) >> 8);        \
    c[1] |=  (n & 0x00FF);              \
}

#define tens(c, n) {                    \
    c[2] &= 0x00;                       \
    c[3] &= 0x03;                       \
    c[2] |= ((n & 0x3FC0) >> 6);        \
    c[3] |= ((n & 0x003F) << 2);        \
}

#define decimal(c, n) {                 \
    c[3] &= 0xFC;                       \
    c[3] |=  (n & 0x0003);              \
}

#define leds(c, n) {                    \
    c[4]  = ((n & 0x000F) << 4);        \
}

class LED {
    private:
        #define S_0     0x3F24      // zero
        #define S_1     0x1800      // one
        #define S_2     0x36C0      // two
        #define S_3     0x3CC0      // three
        #define S_4     0x19C0      // four
        #define S_5     0x2DC0      // five
        #define S_6     0x2FC0      // six
        #define S_7     0x2009      // seven
    //  #define S_7     0x0007      // seven
        #define S_8     0x3FC0      // eight
        #define S_9     0x39C0      // nine
        #define S_FILL	0x3FFF		// fill with ones.
        #define S_Oh    0x3F00      // 'O' (oh)
        #define S_Kay   0x038C      // 'K'

        // block to hold our data.
        char display_data[5];

        // where to write to. the path for this devices
        //    if provided at object creation time.
//        SPI* segment_device;
//        byte segment_pin;
        
    public:
    	const char OK = 'o';		// for passing in 'OK'
        enum COLORS {
            OFF    = 0x00,
            RED    = 0x02,
            YELLOW = 0x04,
            GREEN  = 0x08,
            BUTTON = 0x01
        };
        
        LED() {
            // set the memory space to zero and set
            //    the header bit of the buffer.
            freshen(display_data);
        }

        ~LED() {
        }

		void init() {
			SPI.begin();
			SPI.setClockDivider(128);
			SPI.setBitOrder(MSBFIRST);
		}

        void xmit() {
            // open line to segment device
            // writeout char buffer display_data
            SPI.transfer(display_data[0]);
            SPI.transfer(display_data[1]);
            SPI.transfer(display_data[2]);
            SPI.transfer(display_data[3]);
            SPI.transfer(display_data[4]);
        }
        
        void reset() {
        	SPI.transfer(0x80);
        	SPI.transfer(0x00);
        	SPI.transfer(0x00);
        	SPI.transfer(0x00);
        	SPI.transfer(0x01);

        	SPI.transfer(0x00);
        	SPI.transfer(0x00);
        	SPI.transfer(0x00);
        	SPI.transfer(0x00);
        	SPI.transfer(0x00);
        }

        // this is called with an OR mask of the three
        //    colors that the led brick provides:
        //
        //    GREEN | YELLOW | RED would turn them all on.
        //
        // each call to this overwrites the previous
        //    state so that calling light(0) will turn all
        //    of them off.
        void light(int color) {
            leds(display_data, color);
            xmit();
        }

        void decimalp(int& dec_point) {
        	decimal(display_data, dec_point);
        	xmit();
        }

		// for the sake of readability, this is a wrapper
		void direction(int dec_point) {
			decimalp(dec_point);
		}
        
        void display(short cell_number) {
            if (cell_number == OK) {    // matches 'O' (oh)
                tens(display_data, S_Oh);
                ones(display_data, S_Kay);
            } else {
                // store off the ones and tens place into
                //    temp storage. will use the display
                //    buffer, cells 1 and 2, since those
                //    two cells will be completly over-
                //    written by the eencoding.
                display_data[1] = (cell_number % 10);
                display_data[2] = (cell_number / 10);

                // encode the ones position
                switch (display_data[1]) {
                    case 0: ones(display_data,S_0); break;
                    case 1: ones(display_data,S_1); break;
                    case 2: ones(display_data,S_2); break;
                    case 3: ones(display_data,S_3); break;
                    case 4: ones(display_data,S_4); break;
                    case 5: ones(display_data,S_5); break;
                    case 6: ones(display_data,S_6); break;
                    case 7: ones(display_data,S_7); break;
                    case 8: ones(display_data,S_8); break;
                    case 9: ones(display_data,S_9); break;
                    // could we get to default?
                    default: ones(display_data,S_FILL); break;
                } // end (switch(ones))

                // encode the tens position
                switch (display_data[2]) {
                    case 0: tens(display_data,S_0); break;
                    case 1: tens(display_data,S_1); break;
                    case 2: tens(display_data,S_2); break;
                    case 3: tens(display_data,S_3); break;
                    case 4: tens(display_data,S_4); break;
                    case 5: tens(display_data,S_5); break;
                    case 6: tens(display_data,S_6); break;
                    case 7: tens(display_data,S_7); break;
                    case 8: tens(display_data,S_8); break;
                    case 9: tens(display_data,S_9); break;
                    // could we get to default?
                    default: ones(display_data,S_FILL); break;
                }   // end (switch(tens))
            }   // end if
            // data is translated -- send it.
            xmit();
        } // end (display(cell_number))

        void inspect(char buffer[5]) {
        	memcpy(buffer, display_data, 5);
        }
};

#endif // SEGMENT_H
