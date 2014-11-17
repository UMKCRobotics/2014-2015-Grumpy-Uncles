#include <fstream>
#include <string>

class Configurator {
	private:
		char c_round;
		char c_part;
		char c_start;
		char c_end;
		short MYPOS;
		std::string c_filename;

		bool searchRemainingTime;
		
	public:
		Configurator(ArduinoInterface* interface, std::string filename) {
			// create serial connection to <serialport>
			// block on read from arduino for:
			//    byte for start
			//    byte for end
			//    int for speed
			c_round = interface->serialReadBytes(1);
			c_part  = interface->serialReadBytes(1);
			switch(c_round){
				case 1:
					c_start = 48;
					c_end = 9;
					break;
				case 2:
					c_start = 48;
					c_end = 1;
					break;
				case 3:
					c_start = 49;
					c_end = 1;
					break;
			
			}
			MYPOS = 0;
			c_filename = filename;

			// do we keep searching when we reach the end?
			// this will need to be better handled.
			// perhaps some sort of timer that triggers when the
			//    go button is pushed?
			searchRemainingTime = false;
		}

		bool keepGoing() {
			return searchRemainingTime;
		}

		char round() {
			return c_round;
		}

		char part() {
			return c_part;
		}

		char start() {
			return c_start;
		}

		char end() {
			return c_end;
		}

		void storePathToDisk(short map) {
			// open output stream to file
			std::ofstream fout(c_filename.c_str());
			map[MYPOS] = c_start;
			while (map[MYPOS] != c_end) {
				fout << map[map[MYPOS]] << ",";
				map[MYPOS] = map[map[MYPOS]];
			}
			// close output stream
			fout.close();
		}
		
		void loadPathFromDisk(short map) {
		//	std::ifstream fin(c_filename.c_str());
		//	std::string buffer;
		//	
		//	map[0] = c_start;
		//	while(!fin.eof()) {
		//		// store next move into current node
		//		map[map[0]] << (char)fin;
		//		// move cursor to next node
		//		map[0] = map[map[0]];
		//	}
		// What to do if file isn't found?
		}
};
