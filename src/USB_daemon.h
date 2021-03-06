#ifndef USBWAIT_H
#define USBWAIT_H

#include "wait_for_usb.h"

#include <cstdio>		// remove
#include <unistd.h>		// fork, pid
#include <thread>		// C++11 threads

class USB {
	private:
		std::string egg_file;
		std::string pass_off;
		FILE* egg_carton;
		pid_t egg_pid;
		bool running;
		std::thread* chicken;

	public:
		USB(std::string filename) {
			running = false;
			egg_file = "config/" + filename;
			pass_off = filename;

			// EMG: a good programmer wouldn't delete old data,
			//      they would just concatenate the filename and
			//      the current date so that each run is stored
			//      in its own file.
			//
			// force delete of the file in case it exists. ignore
			//    error, we don't care.

			remove(filename.c_str());
			egg_pid = 0;
		}

		~USB() {
			// make sure file descriptor is closed!
			fclose(egg_carton);
			delete(chicken);
		}

		void found_egg(short cell, char egg) {
			if (egg != '\0') {
				std::cout << "DAEMON :: found_egg --> got " << cell
				          << ", " << egg << std::endl;
				// open file in 'append' mode. this places the seek
				//    pointer directly at the end.
				std::ofstream fout(egg_file.c_str(), std::ofstream::app);
				fout << cell << " " << egg << std::endl;
				fout.close();
			}
			// a good programmer will return the number of
			//    bytes written, or some other error checking.
		}

		bool isRunning() {
			return(running);
		}

		void run() {
			if (not running) {
				// start the thread using wait_for_usb();
				std::cout << "DAEMON :: run() --> starting the USB daemon, passing" 
				          << pass_off << std::endl;
			//	std::thread chicken(wait_for_usb, egg_file.c_str());
				chicken = new std::thread(wait_for_usb, pass_off.c_str());
			//	chicken->detach();
				running = true;
			}
		}

		void join() {
			if (running) {
				chicken->join();
			}
		}
};

#endif // USBWAIT_H
