#include <iostream>		// standard I/O
#include <fstream>		// file I/O
#include <string>		// strings
#include <cstdlib>		// exit

#include "Navigation.h"
#include "Filenames.h"
#include "ArduinoInterface.h"
#include "Configurator.h"
#include "OCR.h"
#include "LEDS.h"
#include "USB_daemon.h"

#define nop() void()

int main(void) {
	// if the UDEV symlink exists, use /dev/ardino
	// if not, change this to  /dev/ttymxc3
	const std::string serialport = "/dev/arduino";

	// point to the value section of the correct gpio
	//    pin that the GO BUTTON is connected to.
	const std::string gopin = "/sys/class/gpio/gpio35/value";

	// generate filenames based on the current date and time.
	//    this allows tracking of files based on run times.
	//    the only way that files could be overwritten is if
	//    the HWCLOCK is reset to 0 ctime after a power-up.
	const std::string egg_carton = filename::generate("config/", "egg");
	const std::string map_file   = filename::generate("config/", "map");

	// OpenCV is pretty easy about its camera source.
	//    you can (if you so desire) provide a movie
	//    file (in any of AVI, MOV, MPG, etc) to test
	//    the same thing over and over. for live, we'll
	//    use the number zero which is the first camera
	//    device that OpenCV finds.
	// this uses some internal video4linux routines to
	//    discover what this is. the udoo will only have
	//    one camera attached, so this is safe.
	const int cameradevice = 0;
	std::cout << "MAIN :: up and running. beginning initializations.\n";

	ArduinoInterface* arduino = new ArduinoInterface(serialport);
	if (arduino ==  NULL) {
		std::cerr << "MAIN :: failed on (arduino <> master) interface\n";
		std::cerr << "MAIN :: FATAL -- bailing.\n";
		exit (11);
	}
	std::cout << "MAIN :: ArduinoInterface up and running.\n";

	Configurator* config = new Configurator(arduino, map_file);
	if (config ==  NULL) {
		std::cerr << "MAIN :: failed on CONFIGURATOR\n";
		std::cerr << "MAIN :: FATAL -- bailing.\n";
		exit (12);
	}
	std::cout << "MAIN :: Configurator up and running.\n";

	OCR* vision = new OCR(cameradevice);
	if (vision ==  NULL) {
		std::cerr << "MAIN :: failed on Camera/OCR interface\n";
		std::cerr << "MAIN :: NON-fatal. continuing.\n";
	//	std::cerr << "MAIN :: FATAL -- bailing.\n";
	//	exit (13);
	}
	std::cout << "MAIN :: Camera up and running.\n";

	LED* marquee = new LED(arduino);
	if (marquee == NULL) {
		std::cerr << "MAIN :: failed on LED interconnect\n";
		std::cerr << "MAIN :: This is sort-of fatal. bailing.\n";
		exit (14);
	}
	std::cout << "MAIN :: Segment interface up and running.\n";

	USB* daemon = new USB(egg_carton);
	if (daemon == NULL) {
		std::cerr << "MAIN :: failed on USB daemon\n";
		std::cerr << "MAIN :: NON-fatal. continuing.\n";
	//	exit (15);
	}
	std::cout << "MAIN :: USB Daemon up and running.\n";

	// map is an array of CARDINALS, in human-redable form:
	//
	//    N, W, S, E
	//
	// the empty braces initialize the array to all zeroes.
	// map[0] will contain a logical number referring to the cell
	//    that the robot currently occupies. when storing, cast
	//    the number to a (char), whern reading, cast as (short)
	char map[50] = { };
	short cell =  0;
	char cardinal = '?';	// '?' is used as a non-value
	char egg = '?';			//     it is also a character
							//     not used as an egg.
	
	// since the arduino half will boot much faster than
	//    our half, we'll need to sync with the arduino
	//    to make sure that we're both on the same page.
	std::cout << "MAIN :: waiting on sync ";
	arduino->sync();
	std::cout << ".. SYNCHED\n";

	config->acquireConfig();

	// ask the configurator where we're starting.
	cell = config->start();
	map[0] = (char)cell;

	std::cout << "MAIN :: config acquired.\n";
	// indicate to the operator that we are 'READY' and
	//    waiting for the go-button to be pressed.
	marquee->light(LED::BUTTON);

	std::cout << "MAIN :: Ready. Waiting for 'GO'\n";
	// do nothing until the button is pressed.
	//    this blocks while waiting.
	config->wait_on_go(gopin.c_str());

	std::cout << "MAIN :: And we're off.\n";
	// the button has been pressed. let's start with
	//    turning the go light on.
	marquee->light(LED::GREEN);

	// part 2 of any round
	// not much to this so it's written first.
	if (config->part() == 2) {

		// the map is returned in order, as written to
		//    the file and null-terminated.
		config->loadPathFromDisk(map);
		for(int cth = 0; map[cth] != '\0'; cth++) {
			// get the next move in the path
			cardinal = navigation::moveto(map[cth]);

			// instruct the robot to move. this call blocks
			//    until the robot is done moving.
			arduino->moveCardinal(cardinal);
		}

	// part 1 of any round
	// this is where a lot of the work is done.
	} else {
		// start the USB daemon thread
		// this better not block!
		if (vision) {
			daemon? daemon->run() : nop();
		} else {
			// light up the yellow to indicate that
			//    the camera did not start up.
		//	marquee->light(LED::GREEN | LED::YELLOW);
		}

		#define EVER ;;
		for (EVER) {
			// tell the arduino to make one move. if there's an egg
			//    in the start cell, we won't catch it unless we
			//    return there.
			arduino->proceed();
		
			// from the above, that means we just block on this line:
			// this line waits on the lower half to tell us that we've
			//    moved. the lower half returns the cardinal that was
			//    used to move - we then need to translate and store
			//    the cell number that we've moved to.
			cell = (short)arduino->readByte();

			// take where we were (map[0]) and find out where we are
			//    based on the cardinal returned.
			cardinal = navigation::moved((short)map[0], cell);

			// navigation::moved returns 0 if we didn't move. in that
			//    case, we should only add the cell we from below if
			//    the returned value is not zero.
			if (cardinal not_eq 0) {
				// keep track of where we've been so that we can
				//    eventually solve this maze.
				navigation::add_to_path(map, cell, cardinal);
			}

			// do we need to take a picture?
			// first, check to see if the camera opened.
			// also check for the daemon. if neither are
			//    opened, there's no point in doing this.
			if (vision && daemon) {
				switch (config->round()) {
					case 2: case 3:
						egg = vision->scan();

						// since the daemon already handles the file
						//    just pass whatever was found to write out.
						daemon->found_egg(cell, egg);
						break;
				}
			}

			// have we reached the end cell?
			if (cell == config->end()) {
				marquee->light(LED::RED);
				config->storePathToDisk(map);
				if (config->keepGoing()) {
					continue; // from the top of the for loop
				} else {
					break; // out of the for loop and stop
				}
			}

			// to be complete, we also check to see if we've
			//    returned to the start cell.
			if (cell == config->start()) {
				marquee->light(LED::YELLOW | LED::RED);
				// however, we should do more than just change the
				//    LEDs. perhaps a stop, or a deadloop?
				//    returning for now.
				return(0);
			}
		} // end run
	}
}
