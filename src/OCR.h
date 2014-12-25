#include <opencv2/highgui/highgui.hpp>
#include <tesseract/baseapi.h>
#include <string>
#include <sstream>
#include <iostream>

class OCR {
	private:
		cv::VideoCapture* capture;
		tesseract::TessBaseAPI* tessa;
		cv::Mat frame;

	public:
		OCR(int dev) {
			capture = new cv::VideoCapture(dev);
			if (!capture->isOpened()) {
				std::cerr << "OCR :: Failure to open default camera device '0'\n";
				std::cerr << "OCR :: non-fatal, Bailing (for now)\n";
				exit(31);
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

		// thread?
		char scan() {
			// clear the buffer of any stray frames.
			// EMG: I learned this 2013 with Wesley.
			for (short dump = 0; dump < 10; dump++) {
				*capture >> frame;
			}
			// capture a fresh (and so clean) frame from the camera
			*capture >> frame;
			// insert  openCV magic to clean up the image.

			// do the tesseract thing.
			tessa->SetImage( (uchar*)frame.data,
			                         frame.size().width,
			                         frame.size().height,
			                         frame.channels(),
			                         frame.step1()
			               );
			tessa->Recognize(0);

			// retreive all the output from tessa
			std::string output(tessa->GetUTF8Text());
			// use tesseracts confidence values to get
			//    the best match
			// https://code.google.com/p/tesseract-ocr/wiki/APIExample
			//
			//	  Pix *image = pixRead("/usr/src/tesseract-3.02/phototest.tif");
			//	  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
			//	  api->Init(NULL, "eng");
			//	  api->SetImage(image);
				// where are a list of the parameters that can be set?
			//	  api->SetVariable("save_blob_choices", "T");
			//	  api->SetRectangle(37, 228, 548, 31);
			//	  api->Recognize(NULL);
			//	

				// ri returns a 'word'
			//	  tesseract::ResultIterator* ri = api->GetIterator();
			//	  tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;
			//	  if(ri != 0) {
			//	      do {
			//	          const char* symbol = ri->GetUTF8Text(level);
			//	          float conf = ri->Confidence(level);
			//	          if(symbol != 0) {
			//	              printf("symbol %s, conf: %f", symbol, conf);
			//	              bool indent = false;
			//	              tesseract::ChoiceIterator ci(*ri);

				// and i think this looks at the individual letters/symbols
			//	              do {
			//	                  if (indent) printf("\t\t ");
			//	                  printf("\t- ");
			//	                  const char* choice = ci.GetUTF8Text();
			//	                  printf("%s conf: %f\n", choice, ci.Confidence());
			//	                  indent = true;
			//	              } while(ci.Next());
			//	          } // end if(symbol)
			//	          printf("---------------------------------------------\n");
			//	          delete[] symbol;
			//	      } while((ri->Next(level)));
			//	  } // end if(ri)

			// this stringstream will allow us to filter
			//    the result -- so hacky.
			std::stringstream ss;
			ss << output;
			output.clear();
			// grab the first non-whitespace character found
			ss >> output;

			return (*output.c_str());
		}
};
