#define CAMERA_MODE 0//WebCam 0,Basler 1

#include <conio.h>
#ifdef _DEBUG
#pragma comment( lib, "opencv_world412d" )
#else
#pragma comment( lib, "opencv_world412" )
#endif


#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <iostream>

#include"face_tracker.h"

#ifdef _DEBUG
#define CAMERA_EXT "d.lib"
#else
#define CAMERA_EXT ".lib"
#endif
/*
#include <HSC/baslerClass.hpp>
#pragma comment(lib, "BaslerLib" CAMERA_EXT)
*/
#define STR_EXP(__A) #__A
#define STR(__A) STR_EXP(__A)
#define CV_VER STR(CV_VERSION_MAJOR) STR(CV_VERSION_MINOR) STR(CV_VERSION_REVISION)
#pragma comment(lib, "opencv_world" CV_VER CAMERA_EXT)
#pragma comment(lib, "BaslerLib" CAMERA_EXT)


using namespace dlib;
using namespace std;


// Load face detection and pose estimation models.
frontal_face_detector detector = get_frontal_face_detector();
shape_predictor sp;

cv::VideoCapture cap(0);
//basler cam;

float p = 0.2;

cv::Mat img;
//dlib::image_window  win;
int main() {
	
	//dataset
	deserialize("shape_predictor_68_face_landmarks.dat") >> sp;

	
	if (CAMERA_MODE) {
		/*
		FaceTracker facetracker(cam);
		std::thread camera([&] {
			while (1) {
				cam.captureFrame(img.data);
			};
		});
		while (1) {
			if (img.data != NULL) facetracker.face_tracking(img,sp,detector,p);
		}
		if (camera.joinable())camera.join();
		*/
	}


	else {

		if (!cap.isOpened())
		{
			cerr << "Unable to connect to camera" << endl;
		}

		FaceTracker facetracker;

		std::thread camera([&] {
			while (1) {
				
				cap.read(img);
			};
		});
		std::thread tracking([&]{
			while (1) {
				if (img.data != NULL) facetracker.face_tracking(img, sp, detector,p);
			}
			
		});
		while (1) {
			if (facetracker.ok) {
				
				cv::imshow("result", facetracker.get_result());
				cv::waitKey(1);

			}
		}
		if (camera.joinable())camera.join();
		if (tracking.joinable())camera.join();
	}
	
}


