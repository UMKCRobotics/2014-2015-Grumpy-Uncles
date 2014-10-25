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

		char start
		char end
		short round { 1, 2, 3 }
		short part { 1, 2 }
			bool?
		int default_speed (read from potentiometer)

*/		

	ArduinoInterface interface(FILE serialport);
	Configurator config(interface.serialport);
	OCR vision(FILE cameradevice);

	// part 2 of any round
	if (config.part == 2) {
		// self running
		config.loadPathFromDisk(&map);
		interface.runMaze(savedpath);
	} else {
		#define EVER ;;
		for (EVER) {
		// part 1 of any round
			interface.start();
			nextMove = serialread();
			map[map[0]] = nextMove;
			map[0] = nextMove;
			switch (config.round) {
				case 2: case 3:
					vision.run();
					break;
			}

			if (current_cell == config.end) {
				interface.light(RED);
				config.storepathtoDisk(&map);
				if (config.searchRemainingTime()) {
					continue;
				} else {
					break;
				}
			}
		}
		// end run
	}
}
