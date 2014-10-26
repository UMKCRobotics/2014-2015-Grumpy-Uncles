#include <string>
#include <stringstream>

class OCR {
	private:
		cv::VideoCapture* capture;
		tesseract::TessBaseAPI* tessa;
		cv::Mat frame;

	public:
		OCR(int dev) {
			capture = new VideoCapture(dev);
			if (!capture.isOpened()) {
				std::cerr << "Failure to open default camera device '0'\n";
				std::cerr << "\n\nBailing (for now)\n";
				return(31);
			}

			tessa = new tesseract::TessBaseAPI();
			tessa->Init(NULL, "eng");
			tessa->SetVariable("tessedit_char_whitelist",
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()");
		}

		~OCR() {
			delete(capture);
			delete(tessa);
		}

		void run() {
			// clear the buffer of any stray frames.
			// EMG: I learned this 2013 with Wesley.
			for (short dump = 0; dump < 10; dump++) {
				cap >> frame;
			}
			// capture a fresh (and so clean) frame from the camera
			cap >> frame;
			// do your openCV magic here to clean up the image.

			// do the tesseract thing.
			tessa->SetImage( (uchar*)frame.data,
			                         frame.size().width,
			                         frame.size().height,
			                         frame.channels(),
			                         frame.step1()
			               );
			tessa->Recognize(0);

			// retreive all the output from tessa
			std::string output(tess->GetUTF8Text());
			std::stringstream ss;

			// this stringstream will allow us to filter the result
			ss << output;
			output.clear();
			// grab the first non-whitespace character found
			ss >> output;
			return (output);
		}
}
