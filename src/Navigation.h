#ifndef NAVIGATION_NS
#define NAVIGATION_NS

#include <cstdlib>		// exit()

namespace navigation {

	void add_to_path(char map[], const int cell) {
		short last_post = map[0];
		short distance  = last_post - cell;

		switch(distance) {
			case -7:
				map[cell] = 'S';
				break;
			case -1:
				map[cell] = 'E';
				break;
			case  1:
				map[cell] = 'W';
				break;
			case  7:
				map[cell] = 'N';
				break;
			default:
				std::cerr << "NAV :: add --> failure: distance " << distance
						  << " unrecognized. FATAL.\n";
				exit(20);
		}

		map[0] = (char)cell;

	}

}

#endif
