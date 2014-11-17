#include "ArduinoInterface.cpp"
#include "Configurator.cpp"
#include "OCR.cpp"

#include <iostream>
#include <string>

int main(void){

	// if the UDEV symlink exists, use /dev/ardino
	// if not, use /dev/ttymxc3
	std::string serialport = "/dev/arduino";
	int cameradevice = 0;
	std::map<short, char> eggs;
	short map[50];
	short MYPOS = 0;

	ArduinoInterface* interface = new ArduinoInterface(serialport);
	if (interface ==  NULL) {
		std::cerr << "BORK :: failed on (arduino <> master) interface\n";
		std::cerr << "BORK :: FATAL -- bailing.\n";
		return (30);
	}
	Configurator* config = new Configurator(interface, "config/path.ssv");
	if (config ==  NULL) {
		std::cerr << "BORK :: failed on CONFIGURATOR\n";
		std::cerr << "BORK :: FATAL -- bailing.\n";
		return (30);
	}
	OCR* vision = new OCR(cameradevice);
	if (vision ==  NULL) {
		std::cerr << "BORK :: failed on Camera/OCR interface\n";
		std::cerr << "BORK :: FATAL -- bailing.\n";
		return (30);
	}

	// part 2 of any round
	if (config->part() == 2) {
		// self running
		config->loadPathFromDisk(&map);
		//interface->runMaze(savedpath);
		for(int mth = 0; map[mth] != 00; mth++) {
			arduinoInterface->moveCardinal(map[mth]);
		}
	} else {
		#define EVER ;;
		for (EVER) {
		// part 1 of any round
			interface->light(ArduinoInterface::GREEN);

		// this might be useless. the robot will be waiting on
		//    the 'GO' button to pressed which is attached to
		//    the arduino side anyway.
		//	interface->start();
		// 
		// from the above, that means we just block on this line:
			nextMove = interface->serialreadbytes(1);
		// and let the arduino drive the robot. Master will just be
		//    for housekeeping and starting the camera/OCR.
			map[map[MYPOS]] = nextMove;
			map[MYPOS] = nextMove;
			// do we need to take a picture?
			switch (config->round()) {
				case 2: case 3:
					// if eggs already contains a key matching
					//    our current cell (map[0]) skip.
					// this happens by asking the eggs container
					//    if it knows where our current cell is.
					//    it will return eggs.end() if it does
					//    not exist.
					// so, if this is true, we haven't taken a
					//    picture in this cell yet.
					if (eggs.find(map[MYPOS]) == eggs.end() {
						// fork()?
						eggs.push_back(map[MYPOS], vision->run());
					}
					break;
				default:
					// do nothing.
					break;
			}

			if (current_cell == config->end()) {
				interface->light(ArduinoInterface::RED);
				config->storePathToDisk(&map);
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
