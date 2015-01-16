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
			__u8  rw;
			__u8  lead;
			__u8  width;
			__u32 speed;
		} settings;

		spi_ioc_transfer payload;

		std::string devname;
		int spidev;
		bool OPEN;

	public:
		SPI(std::string devname) : devname(devname) {

		//	this.open();
			if ((spidev = open(devname, flags)) < 0) {
				fprintf(stderr, "SPI :: SPI() --> unable to open %s\n" devname);
				fprintf(stderr, "SPI :: SPI() --> error: %s\n", strerror(errno));
				OPEN = false;

				fprintf(stderr, "SPI :: SPI() --> ultimately not fatal, but bailing.\n");
				exit(51);
			} else {
				OPEN = true;

				// these are magic for now. they should be replaced with tested
				//    and working values later.
				settings.mode = 0x01;
				settings.lead = 0x01;
				settings.width = 0x01;
				payload.bits_per_word = settings.width;
				settings.speed = 10000;
				payload.speed_hz = settings.speed;

				if (ioctl(spidev, SPI_IOC_WR_MODE, &settings.mode) < 0) {
					fprintf(stderr, "SPI :: () -- > failure setting MODE of %s\n", argv[1]);
					exit(12);
				}

				if (ioctl(spidev, SPI_IOC_WR_LSB_FIRST, &settings.lead) < 0) {
					fprintf(stderr, "SPI :: () -- > failure setting BIT JUSTIFICATION of %s\n", argv[1]);
					exit(12);
				}

				if (ioctl(spidev, SPI_IOC_WR_BITS_PER_WORD, &settings.width) < 0) {
					fprintf(stderr, "SPI :: () -- > failure setting WORD LENGTH of %s\n", argv[1]);
					exit(12);
				}

				if (ioctl(spidev, SPI_IOC_WR_MAX_SPEED_HZ, &settings.speed) < 0) {
					fprintf(stderr, "SPI :: () -- > failure setting MAX SPEED of %s\n", argv[1]);
					exit(12);
				}
			}
		}
		
		~SPI() {
			close(devname);
		}

		bool open() {
			return OPEN;
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
