#ifndef NAVIGATION_NS
#define NAVIGATION_NS

#include <cstdlib>		// exit()

namespace navigation {

	// this function translates the provided cell number
	//    into a cardinal based on the last position
	void add_to_path(char map[], const int cell) {
		short last_post = (short)map[0];
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
				exit(21);
		}

		map[0] = (char)cell;

	}

}

#endif
