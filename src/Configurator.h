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

		// point to the value section of the correct gpio
		//    pin that the GO BUTTON is connected to.
		const char* pin_file = "/sys/class/gpio/gpio35/value";


		ArduinoInterface* arduino;
		bool searchRemainingTime;
		
	public:
		Configurator(ArduinoInterface* interface, std::string mapfile) {
			// the serialib returns values that we should
			//    use for trouble-shooting. see serialib.cpp
			//    for more information.
			arduino = interface;
			MYPOS = 0;
			c_mapfile = mapfile;

			// do we keep searching when we reach the end?
			// configured to false during testing.
			// during actual runs, set to true.
			searchRemainingTime = true;
			c_round = 0;
			c_part = 0;
			c_start = 0;
			c_end = 0;
		}

		void acquireConfig() {
			c_round = (short)arduino->readByte() - '0';
			c_part  = (short)arduino->readByte() - '0';
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
				default:
					std::cerr << "CONFIG :: acquire --> failure. c_round(" << c_round << ")" << std::endl;
			}
			std::cerr << "CONFIG :: acquire --> c_round (" << (short)c_round << ")" << std::endl;
			std::cerr << "CONFIG :: acquire --> c_part (" << (short)c_part << ")" << std::endl;
			std::cerr << "CONFIG :: acquire --> c_start (" << c_start << ")" << std::endl;
			std::cerr << "CONFIG :: acquire --> c_end (" << c_end << ")" << std::endl;
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
			std::cout << "CONFIG :: store() --> Path is: ";
			std::ofstream fout(c_mapfile.c_str());
			map[MYPOS] = (char)c_start;
			while (map[MYPOS] != (char)c_end) {
				std::cout << map[map[MYPOS]] << " ";
				fout << map[map[MYPOS]] << " ";
				map[MYPOS] = (char)navigation::moved((short)map[MYPOS], map[map[MYPOS]]);
			}
			std::cout << std::endl;
			fout << std::endl;
			// close output stream
			fout.close();
		}

		void printMap(char map[]) {
			std::cout << "CONFIG :: store() --> map:" << std::endl;
			for (int rth = 0; rth < 7; rth++) {
				std::cout << "\t";
				for (int cth = 0; cth < 7; cth++) {
					if (map[(rth*7) + cth + 1] == 0) {
						std::cout << ".";
					} else {
						std::cout << map[(rth*7) + cth + 1];
					}
					std::cout << "  ";
				}
				std::cout << std::endl;
			}
		}
		
		void loadPathFromDisk(char map[]) {
			std::cout << "CONFIG :: load() --> loading path from disk" << std::endl;
			std::ifstream fin(c_mapfile.c_str());
			
			short step = 0;
			while (fin >> map[step++]);
		// What to do if file isn't found?
			fin.close();
		}

		bool wait_on_go() {
			// read a GPIO pin for the 'GO' button.
			// return a value based on its state
			// I hope we can plug the btn into a GPIO.
			char value = '1';
			int gopin = open(pin_file, O_RDONLY);

			// block, until the button is pressed.
			do {
				read(gopin, &value, 1);
				// we're reading a file, not just looking
				//    at values like in an arduino, so we
				//    need to reset the file pointer back
				//    to the front of the file.
				lseek(gopin, 0, SEEK_SET);
				usleep(250);
			} while (value == '1');
			// '1' is what the board registers an
			//    unpressed button as.
			std::cerr << std::endl;
			close(gopin);
			return true;
		}
};

#endif // CONFIGURATOR_H
