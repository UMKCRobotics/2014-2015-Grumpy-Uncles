#include <linux/spi/spidev.h> 	// usermode SPI device
#include <sys/ioctl.h> 			// i/o system call

#include <string>

/* on the udoo, the SPI we are using are at:
 *
 *    51 - MOSI aka data
 *    52 - CLK
 *    53 - SS2 aka enable
 *
 */

class SPI {
	private:
		int response;
		std::string dev_name;

	public:
		SPI(std::string device) {
			dev_name = device;
			
		}
};
