#ifndef CARDINAL
#define CARDINAL


namespace dir {
	enum Cardinal {
		START_OF_LIST,
		NORTH,
		EAST,
		SOUTH,
		WEST,
		END_OF_LIST
	};

	// special behavior for ++Cardinal
	Cardinal& operator++(Cardinal &c ) {
		c = static_cast<Cardinal>( static_cast<int>(c) + 1 );
		if (c == END_OF_LIST)
			c = NORTH;
		return c;
	}

	// Special behavior for Cardinal++
	Cardinal operator++(Cardinal &c, int ) {
		Cardinal result = c;
		++c;
		return result;
	}

	Cardinal& operator--(Cardinal &c ) {
		c = static_cast<Cardinal>( static_cast<int>(c) + 1 );
		if (c == START_OF_LIST)
			c = WEST;
		return c;
	}

	// Special behavior for Cardinal++
	Cardinal operator--(Cardinal &c, int ) {
		Cardinal result = c;
		--c;
		return result;
	}

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
