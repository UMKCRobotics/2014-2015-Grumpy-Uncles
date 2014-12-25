#include <stdint.h> 	// uint16_t
#include <string>

class LED {
	private:
		#define START	0x0001
		#define S_0		0x093F		// zero
		#define S_1		0x0006		// one
		#define S_2		0x00DB		// two
		#define S_3		0x0084		// three
		#define S_4		0x00E7		// four
		#define S_5		0x00AD		// five
		#define S_6		0x00FD		// six
		#define S_7		0x2401		// seven
	//	alternate 7
	//	#define S_7		0x0007		// seven
		#define S_8		0x00FF		// eight
		#define S_9		0x00E7		// nine
		#define S_Oh	0x003F		// 'O' (oh)
		#define S_Kay	0x0C70		// 'K'
		#define OK		79			// character 'o'

		// http://en.wikipedia.org/wiki/Bit_field
		// http://en.cppreference.com/w/cpp/language/bit_field
		struct display_t {
			uint16_t header :  2;
			uint16_t ones   : 14;
			uint16_t tens   : 14;
			uint16_t decimal:  2;
			uint8_t  leds   :  4;
			uint8_t		 :  4;
			// (40?)   total: 40.
		} display_data;

		FILE segment_device;
		
	public:
		enum COLORS {
			RED = 1,
			YELLOW = 2,
			GREEN = 4
		};
		
		LED(std::string devname) {
			// open the device
			//segment_device = devname;

			// this is constant.
			display_data.header = 0x01;
		}
		~LED();

		void xmit() {
			// open line to segment device
			// write(display_data);
		}
		
		void light(int color) {
			display_data.leds = color;
			xmit();
		}
		
		void display(short cell_number) {
			if (cell_number == OK) {	// matches 'O' (oh)
				display_data.tens = S_Oh;
				display_data.ones = S_Kay;
			} else {
				// grab the ones place
				display_data.ones = (cell_number % 10);
				// grab the tens place
				display_data.tens = (cell_number / 10);

				switch (display_data.ones) {
					case  0: display_data.ones = S_0; break;
					case  1: display_data.ones = S_1; break;
					case  2: display_data.ones = S_2; break;
					case  3: display_data.ones = S_3; break;
					case  4: display_data.ones = S_4; break;
					case  5: display_data.ones = S_5; break;
					case  6: display_data.ones = S_6; break;
					case  7: display_data.ones = S_7; break;
					case  8: display_data.ones = S_8; break;
					case  9: display_data.ones = S_9; break;
					// could we get to default?
					default: break;
				} // end (switch(ones))
				switch (display_data.tens) {
					case  0: display_data.tens = S_0; break;
					case  1: display_data.tens = S_1; break;
					case  2: display_data.tens = S_2; break;
					case  3: display_data.tens = S_3; break;
					case  4: display_data.tens = S_4; break;
					case  5: display_data.tens = S_5; break;
					case  6: display_data.tens = S_6; break;
					case  7: display_data.tens = S_7; break;
					case  8: display_data.tens = S_8; break;
					case  9: display_data.tens = S_9; break;
					// could we get to default?
					default: break;
				}   // end (switch(tens))
			}   // end if
			// data is translated -- send it.
			xmit();
		} // end (display(cell_number))
};
