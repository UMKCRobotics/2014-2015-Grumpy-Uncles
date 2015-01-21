#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <fstream>
#include <string>

class Configurator {
	private:
		unsigned char c_round;
		unsigned char c_part;
		short c_start;
		short c_end;
		short MYPOS;
		short result;
		std::string c_mapfile;

		bool searchRemainingTime;
		
	public:
		Configurator(ArduinoInterface* interface, std::string mapfile) {
			// the serialib returns values that we should
			//    use for trouble-shooting. see serialib.cpp
			//    for more information.
			interface->sync();
			result = interface->readByte(&c_round);
			result = interface->readByte(&c_part);
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
			c_mapfile = mapfile;

			// do we keep searching when we reach the end?
			// configured to false during testing.
			// during actual runs, set to true.
			searchRemainingTime = false;
		}

		bool keepGoing() {
			return searchRemainingTime;
		}

		short round() {
			return c_round;
		}

		short part() {
			return c_part;
		}

		short start() {
			return c_start;
		}

		short end() {
			return c_end;
		}

		void storePathToDisk(char map[]) {
			// open output stream to file
			std::ofstream fout(c_mapfile.c_str());
			map[MYPOS] = c_start;
			while (map[MYPOS] != c_end) {
				fout << map[map[MYPOS]] << " ";
				map[MYPOS] = map[map[MYPOS]];
			}
			// close output stream
			fout.close();
		}
		
		void loadPathFromDisk(char map[]) {
			std::ifstream fin(c_mapfile.c_str());
			std::string buffer;
			
			short step = 0;
			while (fin >> map[step++]);
		// What to do if file isn't found?
			fin.close();
		}

		bool wait_on_go() {		// STUB
			// read a GPIO pin for the 'GO' button.
			// return a value based on its state
			// I hope we can plug the btn into a GPIO.

			return true;
		}
};

#endif // CONFIGURATOR_H
