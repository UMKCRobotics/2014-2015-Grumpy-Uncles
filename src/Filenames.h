#include <ctime>
#include <string>
#include <sstream>

namespace filename {

	time_t raw;
	struct tm* timeinfo;
	char buffer[20];
	bool set = false;

	void init() {
		time (&raw);
		timeinfo = localtime(&raw);

		strftime(buffer, 20, "%Y%m%d.%H%M%S", timeinfo);
	}

	std::string generate(std::string path, std::string extension) {
		if (not set) {
			set = true;
			filename::init();
		}

		std::stringstream ss;
		ss << path << buffer << "." << extension;
		return (ss.str());
	}
}
