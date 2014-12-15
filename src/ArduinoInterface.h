#include <fstream>
#include <string>

#include "Navigation.h"
#include "serialib.h"

#define ms(s) (1000 * s)

class ArduinoInterface {
	private:
		serialib arduino;
		unsigned int timeout;
		unsigned int baud;

		char call;
		char response;

		unsigned char OP_START;
		unsigned char OP_OK;
		unsigned char OP_SYN;
		unsigned char OP_ACK;

	public:
		ArduinoInterface(std::string serialport) {
			// set the time out. this is in milliseconds
			timeout = (ms(6));

			// baud rate. Udoo seems to prefer 115200.
			baud = 115200;

			if((arduino.Open(serialport.c_str(), baud)) != 1) {
				std::cerr << "ARD_IF :: cannot open arduino device: " \
				          << serialport << std::endl;
				std::cerr << "ARD_IF :: this is very fatal. bailing.\n";
				exit(41);
			}

			// trying to stay away from 0xFF .. -1 might
			//    behave oddly in some situations.
			OP_START = 0xE0;
			OP_OK    = 0xE1;
			OP_SYN   = 0xE2;
			OP_ACK   = 0xE3;
		}

		~ArduinoInterface() {
			arduino.Close();
		}

		char readByte() {
			do {
				response = arduino.ReadChar(&call, timeout);
				// return values:
				// 
				//    -1 : failure
				//     0 : timeout
				//     1 : success
			} while (response <= 0);

			return(call);
		}

		void writeByte(char message) {
			arduino.WriteChar(message);
		}

		// no idea if this will work. but I think the idea
		//    is solid.
		// also not sure if this is even needed.
		bool sync() {
			do {
				this->writeByte(OP_SYN);
			} while((this->readByte()) != OP_ACK);
		}

		void moveCardinal(char cardinal) {
			this->writeByte(cardinal);
			do {
				arduino.ReadChar(&call, timeout);
				// until the lower half says it's ok,
				//    we don't do anything.
			} while(call != OP_OK);
		}

		void proceed() {
			this->writeByte(OP_START);
		}
};
