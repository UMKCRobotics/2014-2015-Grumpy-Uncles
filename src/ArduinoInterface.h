#ifndef ARDUINOIF_H
#define ARDUINOIF_H

#include <fstream>
#include <string>

#include "Navigation.h"
#include "serialib.h"

#define sectoms(s) (1000 * s)

class ArduinoInterface {
	private:
		serialib arduino;
		unsigned int timeout;
		unsigned int baud;

		char call;
		char response;

		unsigned char OP_MOVE;
		unsigned char OP_OK;
		unsigned char OP_SYN;
		unsigned char OP_ACK;

	public:
		ArduinoInterface(std::string serialport) {
			// set the time out. timeout expects milliseconds
			timeout = (sectoms(6));

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
			OP_MOVE = 'm';
			OP_OK   = 'o';
			OP_SYN  = 's';
			OP_ACK  = 'a';
		}

		~ArduinoInterface() {
			arduino.Close();
		}

		// ALL READS WILL BLOCK. make sure that you are
		//    aware of this before you use this function.
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
		void sync() {
			writeByte(OP_SYN);
			while((readByte()) != OP_ACK);
			// a better way to do this is time out,
			//    flush the line if possible and then
			//    restart from the top of sync()
			std::cerr << "ARD_IF :: sync --> SYNC achieved. proceed.\n";
		}

		void moveCardinal(char cardinal) {
			this->writeByte(cardinal);
			do {
				call = this->readByte();
				// until the lower half says it's ok,
				//    we don't do anything.
			} while(call != OP_OK);
		}

		void proceed() {
			this->writeByte(OP_MOVE);
		}
};

#endif // ARDUINOIF_H
