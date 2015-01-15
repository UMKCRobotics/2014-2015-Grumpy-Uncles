#ifndef NAVIGATION_NS
#define NAVIGATION_NS

#include <cstdlib>		// exit()

namespace navigation {
	short moved(short last, const char cardinal) {
		switch(cardinal) {
			case 'S': return (last + 7); break;
			case 'E': return (last + 1); break;
			case 'W': return (last - 1); break;
			case 'N': return (last - 7); break;
			// we should assume we didn't move.
			default:  return (last);     break;
		}
	}

	// this function translates the provided cell number
	//    into a cardinal based on the last position
	void add_to_path(char map[], const short cell, const char cardinal) {
		// map[0]:   where i was
		// cell:     where i am
		// cardinal: where i moved

		map[(short)map[0]] = cardinal;
		map[0] = (char)cell;
	}

}

#endif
