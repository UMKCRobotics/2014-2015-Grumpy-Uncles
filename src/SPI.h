#ifndef SPIDEV_H
#define SPIDEV_H

#include <stdio.h>			// basic i/o
#include <fcntl.h> 			// file i/o
#include <stdlib.h>			// exit
#include <sys/ioctl.h> 		// ioctl
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <errno.h>

#include <string>

class SPI {
	private:
		struct _settings {
			__u8  rwmode;
			__u8  justification;
			__u8  width;
			__u32 speed;
		} settings;

		spi_ioc_transfer payload;

		std::string devname;
		int spidev;
		int flags;

	public:
		SPI(std::string devname) : devname(devname) {
			flags = O_WRONLY;

		//	this.open();
			if ((spidev = open(devname.c_str(), flags)) < 0) {
				std::cerr << "SPI :: SPI() --> unable to open " << devname << std::endl;
				std::cerr << "SPI :: SPI() --> error: " << strerror(errno) << std::endl;
				std::cerr << "SPI :: SPI() --> ultimately not fatal, but bailing.\n";
				exit(51);
			} else {
				// these are magic for now. they should be replaced with tested
				//    and working values later.
				settings.rwmode = 0x01;
				settings.justification = 0x01;
				settings.width = 0x01;
				payload.bits_per_word = settings.width;
				settings.speed = 10000;
				payload.speed_hz = settings.speed;

				if (ioctl(spidev, SPI_IOC_WR_MODE, &settings.rwmode) < 0) {
					std::cerr << "SPI :: () -- > failure setting MODE" << std::endl;
					exit(52);
				}

				if (ioctl(spidev, SPI_IOC_WR_LSB_FIRST, &settings.justification) < 0) {
					std::cerr << "SPI :: () -- > failure setting BIT JUSTIFICATION" << std::endl;
					exit(53);
				}

				if (ioctl(spidev, SPI_IOC_WR_BITS_PER_WORD, &settings.width) < 0) {
					std::cerr << "SPI :: () -- > failure setting WORD LENGTH" << std::endl;
					exit(54);
				}

				if (ioctl(spidev, SPI_IOC_WR_MAX_SPEED_HZ, &settings.speed) < 0) {
					std::cerr << "SPI :: () -- > failure setting LINE SPEED" << std::endl;
					exit(55);
				}
			}
		}
		
		~SPI() {
			close(spidev);
		}

		void write(char buffer[]) {
			payload.tx_buf = buffer;
			// cheating since it's a char buffer.
			payload.len = sizeof(buffer);

			if ((ioctl(devname, SPI_IOC_MESSAGE(1), payload)) < 0) {
				fprintf(stderr, "SPI :: write --> Unable to write message.\n");
				fprintf(stderr, "SPI :: write --> Segment display NOT fatal.\n");
			}
		}
			
};

#endif // SPIDEV_H
