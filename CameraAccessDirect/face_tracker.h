#pragma once
#ifndef _FACE_TRACKER_H_
#define _FACE_TRACKER_H_
#include<opencv2/opencv.hpp>
//#include <HSC/baslerClass.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include <dlib/gui_widgets.h>
#include<vector>

class FaceTracker
{
public:
	FaceTracker();
	bool ok = false;
	//FaceTracker(basler cam);
	void FaceTracker::face_tracking(cv::Mat img,dlib::shape_predictor sp, dlib::frontal_face_detector detector,float p);
	cv::Mat FaceTracker::get_result();
private:
	cv::Size2i image_size;
	float fps;
	float gain;
	

	cv::Mat image,face_image,rotation_image,result;
	float image_degree = 0.0f,degree=0.0f;
	cv::Point2f nose,roi_origin;
	cv::Size2i velonica,roi_size;
	std::vector<dlib::rectangle> faces,face;
	cv::Point2i rect1,rect2,rect3,rect4;
	float down_ratio=1.0f;
	
};



#endif // !_FACE_TRACKER_H_
