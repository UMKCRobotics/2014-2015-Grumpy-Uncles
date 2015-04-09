#ifndef SPIGR_H
#define SPIGR_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h> 			// close
#include <stdlib.h>
#include <fcntl.h> 				// open
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <errno.h>
#include <string.h>

#include <ios>
#include <iostream>

class SPI {
	private:
		uint32_t rwmode;
	//	__u8  justification;

		spi_ioc_transfer payload;

		int spidev;
		int status;

	public:
		SPI(std::string devname) {
			rwmode = SPI_MODE_1 | SPI_CS_HIGH;
			// we want MSB first, but there's no define for that.
			// for now, assuming the spi is MSB first by default.
	//		justification = SPI_LSB_FIRST;

			// another option for open flags is O_APPEND
			// maybe also do some reading into O_NONBLOCK
			// maybe also do some reading into (| 0x03)
			//    see "man 2 open"
			spidev = open(devname.c_str(), O_WRONLY);
			if (spidev <= 0) {
				std::cerr << "SPI :: SPI() --> unable to open "
				          << devname << std::endl;
				std::cerr << "SPI :: SPI() --> ultimately NOT fatal. Bailing anyway.\n";
				std::cerr << "SPI :: SPI() --> " << strerror(errno) << std::endl;
				exit(51);
			}

			std::cerr << "SPI :: SPI() --> open(" << spidev << ")\n";

			// should confre with Darren about the correct mode
			status = ioctl(spidev, SPI_IOC_WR_MODE, &rwmode);
			if (status < 0) {
				std::cerr << "SPI :: SPI() --> unable to set mode of SPI device.\n";
				std::cerr << "SPI :: SPI() --> treating this as fatal; bailing.\n";
				std::cerr << "SPI :: SPI() --> " << strerror(errno) << std::endl;
				exit(52);
			}

			// for now, only setting the mode explictly. a good
			//    programmer would do it all at once and not ever
			//    worry about it again.
		}

		~SPI() {
			close(spidev);
		}

		void send(char buffer[], const int length) {
			std::cerr << "SPI :: send --> entered with:";
			for (int dth = 0; dth < length; dth++) printf(" %#04x", buffer[dth]);
			std::cerr << std::endl;

			// EMG: i don't trust this cast. the spidev header states:
			//
			//    #tx_buf: Holds pointer to userspace buffer with
			//             transmit data, or null.
			//
			// spilib.c uses (unsigned long)

			payload.tx_buf = (unsigned long)&buffer;
			payload.len = length;

			status = ioctl(spidev, SPI_IOC_MESSAGE(1), &payload);
			if (status < 0) {
				std::cerr << "SPI :: send --> error sending payload to device.\n";
				std::cerr << "SPI :: send --> not fatal, but not useful. bailing.\n";
				std::cerr << "SPI :: send --> " << strerror(errno) << std::endl;
				exit(53);
			}
		}
};

#endif // SPIGR_H
