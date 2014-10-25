class Configurator {
	private:
		short c_round;
		short c_part;
		char c_start;
		char c_end;
		string filename;
		
	public:
		Configurator(FILE serialport, string filename) {
			// create serial connection to <serialport>
			// block on read from arduino for:
			//    byte for start
			//    byte for end
			//    int for speed
			serialreadbyte(serialport, c_round);
			serialreadbyte(serialport, c_part);
			serialreadbyte(serialport, c_start);
			serialreadbyte(serialport, c_end);

			c_filename = filename;
		}

		short round() {
			return c_round;
		}

		short part() {
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
