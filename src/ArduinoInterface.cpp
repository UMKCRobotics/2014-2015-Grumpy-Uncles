#include <fstream>
#include <string>

class ArduinoInteface {
	private:
		ifstream ar_in;
		ofstream ar_out;

	public:
		ArduinoInterface(string serialport) {
			// connect to serialport
			//    baud 115200 -- do NOT change this!
			ar_in.open(serialport);
			ar_out.open(serialport);

			// or, in C:
			// this will open the file as read/write in a single
			//    file descriptor.
			/*
			 * int arduino = open(serialport.c_str(), O_RDWR);
			 *
			 */

			// would probably be better to just use a third-party
			//    serial library that can handle this. Connor gave
			//    me a link to libserial(?). Will look into it.

		}

		// used in part 1
		void start() {
			serialWriteBytes(1, 0xFF);
			// light GREEN and return nextCell as 64. this value
			//    could be interpreted internally as the STARTCODE.
			//
			// I'm hoping that the 0xFF value (or just 0x3F) can
			//    be reserved for this functionality.
		}

		// used in part 2
		void runMaze() {
			serialWriteBytes(1, 0xFE);
			// light GREEN and return nextCell as 63.
			//
			// I'm hoping that the value 0xFE (or just 0x3E) can
			//    be reserved for this functionality.
		}

		int serialReadBytes(int count) {
			// read bytes until count hits 0.
			// return number of bytes read
			return (count);
		}

		int serialWriteBytes(int count, char* output) {
			// write bytes until count hits 0?
			// write bytes until char array hits end?
			//
			// maybe this should be overloaded to handle
			//    different data types?

			// return number of bytes written
			return (count);
		}

		// this enum masks the 2 MSB to three different values
		//    representing the three colors on our LED strip.
		//    this allows us to have a single read and write
		//    function for the serial line and encode two
		//    completely different values into a single byte.
		//
		// in the arduino side of the interface you need to do
		//    two different things on each byte that comes in.
		//
		//        switch (incoming_byte & 0xC0) {
		//        // this will capture the two leading bits:
		//            case 0x40:
		//                light RED led
		//            case 0x80:
		//                light YELLOW led
		//            case 0xC0:
		//                light GREEN led
		//            default:
		//                break;
		//        }
		//
		//        // the lower 6 bits would contain the next
		//        //    cell that we need to move to. save that
		//        //    off so we can save the result.
		//        char nextCell = (incoming_byte & 0x3F);
		//        if (nextCell == 0) {
		//            do_nothing();
		//        } else {
		//        // there was something encoded in the lower 6
		//        //    bits: most likely a cell to move to.
		//            moveCardinal(calculateCardnialnextCell));
		//        }
		//
		// it is entirely possible to encode BOTH an LED code
		//    AND a cell in the same byte, but this might not
		//    be useful.
		enum LED_COLOR { RED    = 0x40,
		                 YELLOW = 0x80,
		                 GREEN  = 0xC0 };
		void light(char color) {
			serialWriteBytes(1, color);
		}
}
