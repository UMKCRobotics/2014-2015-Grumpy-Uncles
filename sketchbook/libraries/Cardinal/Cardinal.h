#ifndef CARDINAL
#define CARDINAL

namespace dir {
	enum Cardinal {NORTH, EAST, SOUTH, WEST};

	Cardinal returnCardinal(const char command) {
		switch (command) {
			case 'S':
				return SOUTH;
				break;
			case 'E':
				return EAST;
				break;
			case 'W':
				return WEST;
				break;
			case 'N':
				return NORTH;
				break;
		}
	}
}

#endif
