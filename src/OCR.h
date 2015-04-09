#ifndef OCR_H
#define OCR_H

#include <opencv2/opencv.hpp>
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
				std::cerr << "OCR :: non-fatal, continuing\n";
//				exit(31);
			}

			tessa = new tesseract::TessBaseAPI();
			tessa->Init(NULL, "eng");
			tessa->SetVariable(
				"tessedit_char_whitelist",
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()"
			);
		}

		~OCR() {
			delete(capture);
			delete(tessa);
		}

		bool isOpened() {
			return (capture->isOpened());
		}

		// thread?
		char scan() {
			// clear the buffer of any stray frames.
			// EMG: I learned this 2013 with Wesley.
			for (short dump = 0; dump < 10; dump++) {
				*capture >> frame;
			}
			std::cout << "OCR :: scan --> frames dumped. ready for fresh" << std::endl;
			// capture a fresh (and so clean) frame from the camera
			*capture >> frame;
			std::cout << "OCR :: scan --> frame captured." << std::endl;
			// insert  openCV magic to clean up the image.
			cv::Mat roi = frame(cv::Rect(225, 220, 220, 170));
			// magic happens here
			cv::cvtColor(roi, roi, CV_RGB2GRAY);
			cv::blur(roi, roi, cv::Size(5, 5));
			// threshold value here can and should change due to
			//    lighting conditions at competition.
			int th = 50;
			cv::threshold(roi, roi, th, 255, CV_THRESH_BINARY);
			std::cout << "OCR :: scan --> ROI set and cleaned." << std::endl;

			// do the tesseract thing.
			tessa->SetImage(
				(uchar*)roi.data,
			            roi.size().width,
			            roi.size().height,
			            roi.channels(),
			            roi.step1()
			);
			tessa->Recognize(0);
			std::cout << "OCR --> scan --> tessa working.." << std::endl;

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
			ss.str(output);
			output.clear();
			// grab the first non-whitespace character found
			ss >> output;
			std::cout << "OCR :: scan --> tessa got back: " << output << std::endl;

			#ifdef GDEBUG
			/* compile with -DGDEBUG in order to see what the camera
			 *    sees. You will need access to a graphical head in
			 *    order to see the images.
			 *
			 * when an image is displayed, this code will block until
			 *    the operator presses the escape key (this is to
			 *    allow inspection of the images at your leisure).
			 */
		//	cv::Mat window(frame.rows, frame.cols*2, CV_8UC3);
			cv::namedWindow("xray", CV_WINDOW_AUTOSIZE);
		//	cv::hconcat(frame, roi, window);
			cv::putText(
				frame,
				output,
				cv::Point(200, 400),
				cv::FONT_HERSHEY_PLAIN,
				1.5,
				CV_RGB(0xFF, 0xDF, 0x00),
				2
			);
			cv::imshow("xray", roi);
			while (cv::waitKey() != 27);
			#endif

			// we're only interested in the first character
			return (output[0]);
		//	return (*output.c_str());
		}
};

#endif // OCR_H
