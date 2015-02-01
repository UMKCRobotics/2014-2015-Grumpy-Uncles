#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <cstdlib>		// exit()

namespace navigation {
	enum CARDINAL {
		NORTH = 0,
		EAST  = 1,
		SOUTH = 2,
		WEST  = 3
	};

	char moved(short last, const short cell) {
		switch(last - cell) {
			case -7: return ('N'); break;
			case -1: return ('W'); break;
			case  1: return ('E'); break;
			case  7: return ('S'); break;

			// this indicates that we haven't moved.
			//    perhaps we just turned in place?
			case  0: return (0);   break;
			// can this be triggered?
			default:               break;

			// what about case 0?
		}
	}

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

	char moveto(const char direction) {
		switch (direction) {
			case 'S': return (SOUTH); break;
			case 'E': return  (EAST); break;
			case 'W': return  (WEST); break;
			case 'N': return (NORTH); break;
			// what happens if we get here?
			default:                  break;
		}
	}

	void add_to_path(char map[], const short cell, const char cardinal) {
		// map[0]:   where i was
		// cell:     where i am
		// cardinal: where i moved

		// store how we moved into the location that we were just at
		map[(short)map[0]] = cardinal;
		// store what cell we're in now to our cursor
		map[0] = (char)cell;
	}
}

#endif // NAVIGATION_H
