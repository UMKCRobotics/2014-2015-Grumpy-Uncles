#include <fstream>
#include <string>

class Configurator {
	private:
		char c_round;
		char c_part;
		char c_start;
		char c_end;
		string filename;

		bool searchRemainingTime;
		
	public:
		Configurator(ArduinoInterface* interface, string filename) {
			// create serial connection to <serialport>
			// block on read from arduino for:
			//    byte for start
			//    byte for end
			//    int for speed
			c_round = interface->readBytes(1);
			c_part  = interface->readBytes(1);
			c_start = interface->readBytes(1);
			c_end   = interface->readBytes(1);

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

		void storePathToDisk(char* map) {
			// open output stream to file
			ofstream fout(c_filename);
			map[0] = c_start;
			while (map[0] != c_end) {
				fout << map[map[0]] << ",";
				map[0] = map[map[0]];
			}
			// close output stream
			fout.close();
		}
		
		void loadPathFromDisk(char* map) {
			ifstream fin(c_filename);
			map[0] = c_start;
			string buffer;
			readline(fin, buffer);
			buffer.split(",");
			// this is incorrect ..
			for (int bth = 0; bth < buffer.size(); bth++) {
				map[0] = buffer[bth++];
				map[map[0]] = buffer[bth];
			}
		}
}
