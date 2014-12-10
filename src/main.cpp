#include <iostream>		// standard I/O
#include <fstream>		// file I/O
#include <string>		// strings
#include <cstdlib>		// exit

#include "Navigation.h"
#include "ArduinoInterface.h"
#include "Configurator.h"
#include "OCR.h"
#include "Billboard.h"
#include "USB_daemon.h"

int main(void) {
	// if the UDEV symlink exists, use /dev/ardino
	// if not, use /dev/ttymxc3
	const std::string serialport = "/dev/arduino";
	const std::string billboard  = "/dev/billboard";
	const std::string egg_carton = "config/eggs.txt";
	const std::string path_file  = "confing/path.ssv";
	const int cameradevice = 0;

	// map is an array of CARDINALS, in human-redable form: N, W, S, E
	char map[50];

	ArduinoInterface* interface = new ArduinoInterface(serialport);
	if (interface ==  NULL) {
		std::cerr << "MAIN :: failed on (arduino <> master) interface\n";
		std::cerr << "MAIN :: FATAL -- bailing.\n";
		exit (31);
	}
	Configurator* config = new Configurator(interface, path_file);
	if (config ==  NULL) {
		std::cerr << "MAIN :: failed on CONFIGURATOR\n";
		std::cerr << "MAIN :: FATAL -- bailing.\n";
		exit (32);
	}
	OCR* vision = new OCR(cameradevice);
	if (vision ==  NULL) {
		std::cerr << "MAIN :: failed on Camera/OCR interface\n";
		std::cerr << "MAIN :: FATAL -- bailing.\n";
		exit (33);
	}

	LEDs* marquee = new LEDs(billboard);
	if (marquee == NULL) {
		std::cerr << "MAIN :: failed on LED/Segment displau\n";
		std::cerr << "MAIN :: NON-fatal. continuing\n";
	//	exit (34);
	}

// this object, on instantiation, needs to check for the
//    presence of this file, and delete it if it exists.
	USB* daemon = new USB(egg_carton);
	if (daemon == NULL) {
		std::cerr << "MAIN :: failed on USB daemon\n";
		std::cerr << "MAIN :: NON-fatal. continuing.\n";
	//	exit (35);
	}

	short cell =  0;
	char egg = '|';			// '|' indicates a non-value
							//     it is also a character
							//     not used in the maze
	
	// indicate to the operator that we are 'GO' and waiting for the
	//    button to be pressed.
	marquee->light(LEDs::YELLOW);
	while (config->wait_on_go()) {
		// do nothing until the button is pressed.
	}

	if (config->part() == 2) {
		// part 2 of any round
		// not much to this so it's written first.

		marquee->light(LEDs::GREEN);
		// self running
		config->loadPathFromDisk(map);
		//interface->runMaze(savedpath);
		for(int cth = 0; map[cth] != '\0'; cth++) {
			interface->moveCardinal(map[cth]);
		}
	} else {
		marquee->light(LEDs::GREEN);
		// part 1 of any round
		daemon->run();		// start the USB daemon thread
							// this better not block!
		#define EVER ;;
		for (EVER) {
		
			// from the above, that means we just block on this line:
			// this line waits on the lower half to tell us that we've
			//    moved to another cell. the cell number is passed up
			//    and we use that information for storage and any eggs
			//    we find in this cell.
			cell = interface->serialReadBytes(1);
			marquee->display(cell);

			// and let the arduino drive the robot. Master will just be
			//    for housekeeping and starting the camera/OCR.
			//navigation::addCurrentNodeplusCardinaltoPath(map, cell);
			navigation::add_to_path(map, cell);
			
			// do we need to take a picture?
			switch (config->round()) {
				case 2: case 3:
					// this returns.
					// store this value, and the current node
					//    into a pair<cell, char>, and then
					//    append to a vector.
					//    open file, write file, close file
					//
					// don't need a fancy data structure.
					// just write to the file after every return.
				//	egg = vision->run(map[MYPOS]);
					egg = vision->run();

					// since the daemon already handles the file
					//    just pass this off to write out.
					daemon->found_egg(cell, egg);
					
					// this falls through on PURPOSE (EMG).
				default:
					// regardless of what round we're in, the next
					//    step is to move. We need to let the lower
					//    half know that we're done processing.
					interface->proceed();
					break;
			}

			if (cell == config->end()) {
				marquee->light(LEDs::RED);
				config->storePathToDisk(map);
				if (config->keepGoing()) {
					continue;
				} else {
					break;
				}
			}
		}
		// end run
	}
}
