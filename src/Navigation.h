#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <cstdlib>		// exit()

namespace navigation {
	short moved(short last, const char cardinal) {
		switch(cardinal) {
			// EMG: a good programmer would do
			//      bounds checking
			case 'S': return (last + 7); break;
			case 'E': return (last + 1); break;
			case 'W': return (last - 1); break;
			case 'N': return (last - 7); break;
			// we should assume we didn't move.
			default:  return (last);     break;
		}
	}

	void add_to_path(char map[], const short cell, const char cardinal) {
		// map[0]:   where i was
		// cell:     where i am
		// cardinal: where i moved

		// store, in the location that we were, how we moved
		map[(short)map[0]] = cardinal;
		// store, in our cursor what cell we are in now
		map[0] = (char)cell;
	}
}

#endif // NAVIGATION_H
