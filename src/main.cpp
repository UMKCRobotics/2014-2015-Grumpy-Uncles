#include <iostream>		// standard I/O
#include <fstream>		// file I/O
#include <string>		// strings
#include <cstdlib>		// exit

#include "Navigation.h"
#include "Filenames.h"
#include "ArduinoInterface.h"
#include "Configurator.h"
#include "OCR.h"
#include "Billboard.h"
#include "USB_daemon.h"

int main(void) {
	// if the UDEV symlink exists, use /dev/ardino
	// if not, use /dev/ttymxc3
	const std::string serialport = "/dev/arduino";

	// making a udev rule would be pretty, but this
	//    might end up being /dev/spidevX.Y
	const std::string billboard  = "/dev/billboard";

	// generate filenames based on the current date and time.
	//    this allows tracking of files based on run times.
	//    the only way that files could be overwritten is if
	//    the HWCLOCK is reset to 0 ctime after a power-up.
	const std::string egg_carton = filename::generate("config/", "egg");
	const std::string map_file   = filename::generate("config/", "map");
	const int cameradevice = 0;

	// map is an array of CARDINALS, in human-redable form:
	//
	//    N, W, S, E
	//
	// the empty braces initialize the array to all zeroes.
	char map[50] = { };

	ArduinoInterface* arduino = new ArduinoInterface(serialport);
	if (arduino ==  NULL) {
		std::cerr << "MAIN :: failed on (arduino <> master) interface\n";
		std::cerr << "MAIN :: FATAL -- bailing.\n";
		exit (11);
	}
	Configurator* config = new Configurator(arduino, map_file);
	if (config ==  NULL) {
		std::cerr << "MAIN :: failed on CONFIGURATOR\n";
		std::cerr << "MAIN :: FATAL -- bailing.\n";
		exit (12);
	}
	OCR* vision = new OCR(cameradevice);
	if (vision ==  NULL) {
		std::cerr << "MAIN :: failed on Camera/OCR interface\n";
		std::cerr << "MAIN :: NON-fatal. continuing.\n";
	//	std::cerr << "MAIN :: FATAL -- bailing.\n";
	//	exit (13);
	}

	LED* marquee = new LED(billboard);
	if (marquee == NULL) {
		std::cerr << "MAIN :: failed on LED/Segment display\n";
		std::cerr << "MAIN :: NON-fatal. continuing.\n";
	//	exit (14);
	}
	USB* daemon = new USB(egg_carton);
	if (daemon == NULL) {
		std::cerr << "MAIN :: failed on USB daemon\n";
		std::cerr << "MAIN :: NON-fatal. continuing.\n";
	//	exit (15);
	}

	short cell =  0;
	char egg = '?';			// '?' indicates a non-value
							//     it is also a character
							//     not used in the maze
	
	// indicate to the operator that we are 'GO' and waiting for the
	//    button to be pressed.
	marquee->light(LED::YELLOW);
	while (config->wait_on_go()) {
		// do nothing until the button is pressed.
	}

	// the button has been pressed. let's move out of
	//    the start and begin the maze.
	// is there an easter egg in the start cell?
	//    if so, then we'll need to snap a picture
	//    before we move.
	marquee->light(LED::GREEN);

	if (config->part() == 2) {
		// part 2 of any round
		// not much to this so it's written first.

		// the map is returned in order, as written
		//    to the file.
		config->loadPathFromDisk(map);
		//arduino->runMaze(savedpath);
		for(int cth = 0; map[cth] != '\0'; cth++) {
			// the following call blocks until the robot
			//    is done moving.
			arduino->moveCardinal(map[cth]);
		}
	} else {
		// part 1 of any round
		daemon->run();		// start the USB daemon thread
							// this better not block!
		#define EVER ;;
		for (EVER) {
			// tell the arduino to move one cell
			arduino->proceed();
		
			// from the above, that means we just block on this line:
			// this line waits on the lower half to tell us that we've
			//    moved to another cell. the cell number is passed up
			//    and we use that information for storage and any eggs
			//    we find in this cell.
			cell = arduino->readByte();
			marquee->display(cell);

			// keep track of where we've been so that we can eventually
			//    solve this maze.
			navigation::add_to_path(map, cell);
			
			// hav we reached the end cell?
			if (cell == config->end()) {
				marquee->light(LED::RED);
				config->storePathToDisk(map);
				if (config->keepGoing()) {
					continue; // from the top of the for loop
				} else {
					break; // out of the for loop and stop
				}
			}

			// do we need to take a picture?
			switch (config->round()) {
				case 2: case 3:
					// just in case the camera didn't open.
					if (not (vision == NULL)) {
						egg = vision->scan();
					}

					// since the daemon already handles the file
					//    just pass whatever was found to write out.
					daemon->found_egg(cell, egg);
					break;
			}
		}
		// end run
	}
}
