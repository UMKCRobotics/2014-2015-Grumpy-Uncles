#include <fstream>
#include <string>

#include "Navigation.h"

class ArduinoInterface {
	private:
		std::ifstream ar_in;
		std::ofstream ar_out;

		char OP_START;
		char OP_MAZE;
	public:
		ArduinoInterface(std::string serialport) {
			OP_START = 0xFF;
			OP_MAZE = 0xFE;
			// connect to serialport
			//    baud 115200 -- do NOT change this!
		//	ar_in.open(serialport);
		//	ar_out.open(serialport);

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
			serialWriteBytes(1, &OP_START);
			// light GREEN and return nextCell as 64. this value
			//    could be interpreted internally as the OP_START.
			//
			// I'm hoping that the 0xFF value (or just 0x3F) can
			//    be reserved for this functionality.
		}

		// used in part 2
		void runMaze() {
			serialWriteBytes(1, &OP_MAZE);
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

		void moveCardinal(char cardinal) {
			switch(cardinal) {
				case 'S':
					break;
				case 'E':
					break;
				case 'W':
					break;
				case 'N':
					break;
				default:
					std::cerr << "ARD_IF :: move_cardinal --> bad instruction: "
							  << cardinal << " not understood. SKIPPING\n";
					break;
			}
		}

		void proceed() {
			serialWriteBytes(1, &OP_START);
		}
};
