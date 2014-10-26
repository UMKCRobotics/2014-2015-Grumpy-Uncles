int main(void){

/* ---- turn this into an object ----
	// read configuration values from arduino
	while (readBytes() != null) {
		// store information from the arduino
	}

	// set variables according to the data from the arduino
	char start = 0;
	char end = 0;

	the configurator object will contain at least these attributes:
	// these are 'char' to mach the size constraints
	//    defined inside the arduino code.
		char start
		char end
		char round { 1, 2, 3 }
		char part { 1, 2 }
			bool?

*/		

	string serialport = "/dev/udoo";
	int cameradevice = 0;
	map<char, char> eggs;

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
		interface->runMaze(savedpath);
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
			map[map[0]] = nextMove;
			map[0] = nextMove;
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
					if (eggs.find(map[0]) == eggs.end() {
						// fork()?
						eggs.push_back(map[0], vision->run());
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
