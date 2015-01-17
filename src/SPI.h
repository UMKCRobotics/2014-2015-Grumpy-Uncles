#ifndef SPIGR_H
#define SPIGR_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h> 				// open
#include <unistd.h> 			// close
#include <linux/spi/spidev.h>
#include <stdlib.h>

#include <iostream>

class SPI {
	private:
		__u8  rwmode;
	//	__u8  justification;

		spi_ioc_transfer payload;

		int spidev;

	public:
		SPI(std::string devname) {
			rwmode = SPI_MODE_1;
			// we want MSB first, but there's no define for that.
			// for now, assuming the spi is MSB first by default.
	//		justification = SPI_LSB_FIRST;

			// another option for open flags is O_APPEND
			// maybe also do some reading into O_NONBLOCK
			// maybe also do some reading into (| 0x03)
			//    see "man 2 open"
			if (spidev = open(devname.c_str(), O_WRONLY) < 0) {
				std::cerr << "SPI :: SPI() --> unable to open "
				          << devname << std::endl;
				std::cerr << "SPI :: SPI() --> ultimately NOT fatal. Bailing anyway.\n";
				exit(51);
			}

			// should confre with Darren about the correct mode
			if (ioctl(spidev, SPI_IOC_WR_MODE, &rwmode) < 0) {
				std::cerr << "SPI :: SPI() --> unable to set mode of SPI device.\n";
				std::cerr << "SPI :: SPI() --> treating this as fatal; bailing.\n";
				exit(52);
			}

			// for now, only setting the mode explictly. a good
			//    programmer would do it all at once and not ever
			//    worry about it again.
		}

		~SPI() {
			close(spidev);
		}

		void send(char buffer[]) {
			// EMG: i don't trust this cast. the spidev header states:
			//
			//    #tx_buf: Holds pointer to userspace buffer with
			//             transmit data, or null.
			//
			// spilib.c uses (unsigned long).
			payload.tx_buf = (__u64)buffer;
			payload.len = sizeof(buffer);

			if(ioctl(spidev, SPI_IOC_MESSAGE(1), payload) < 0) {
				std::cerr << "SPI :: xmit --> error sending payload to device.\n";
				std::cerr << "SPI :: xmit --> not fatal, but not useful. bailing.\n";
				exit(53);
			}
		}
};

#endif // SPIGR_H
