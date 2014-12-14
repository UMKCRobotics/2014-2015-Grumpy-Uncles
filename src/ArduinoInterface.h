#include <fstream>
#include <string>

#include "Navigation.h"
#include "serialib.h"

#define ms(s) (1000 * s)

class ArduinoInterface {
	private:
		std::ifstream ar_in;
		std::ofstream ar_out;

		serialib arduino;
		unsigned int timeout;

		char call;
		char response;

		char OP_START;
		char OP_MAZE;
		char OP_OK;

	public:
		ArduinoInterface(std::string serialport) {
			timeout = (ms(6)); // seconds (in ms)
			if((arduino.Open(serialport.c_str(), 115200)) != 1) {
				std::cerr << "ARD_IF :: cannot open arduino device: " \
				          << serialport << std::endl;
				std::cerr << "ARD_IF :: this is very fatal. bailing.\n";
				exit(41);
			}

			OP_START = 0xEE;
			OP_MAZE = 0xDD;
			OP_OK = 0xCC;
		}

		char readByte() {
			// read bytes until count hits 0.
			// return number of bytes read
			do {
				call = arduino.ReadChar(&response, timeout);
			} while (call <= 0);

			return(response);
		}

		void moveCardinal(char cardinal) {
			arduino.WriteChar(cardinal);
			do {
				arduino.ReadChar(&response, timeout);
			} while(response != OP_OK);
		}

		void proceed() {
			arduino.WriteChar(OP_START);
		}
};
