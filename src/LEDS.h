#ifndef LEDS_H
#define LEDS_H

#include "ArduinoInterface.h"

class LED {
	private:
		ArduinoInterface* arduino;

	public:
		enum COLORS {
			OFF    = 0x00;
			BUTTON = 0x01;
			RED    = 0x02;
			YELLOW = 0x04;
			GREEN  = 0x08;
		};

		LED() { };

		~LED() { };

		// using 0xE0 as a command header
		// using 0x0F as a mask to care about only
		//    the low four bits.
		void light(int colors) {
			uint8_t header = 0xE0;
			uint8_t mask = 0x0F;
			uint8_t tunnel = (colors & mask);

			tunnel |= header;
			arduino->writeByte((byte)(0xEF & colors));
		}
};

#endif // LEDS_H
