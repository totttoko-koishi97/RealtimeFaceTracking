#include"face_tracker.h"
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>

#define PI 3.14159265358979323846



using namespace std;
using namespace dlib;
FaceTracker::FaceTracker() {
	
}
/*
FaceTracker::FaceTracker(basler cam) {

	image_size.width = 720;
	image_size.height = 540;
	fps = 500.0f;
	gain = 1.0f;

	cam.connect(0);
	cam.setParam(paramTypeCamera::paramInt::WIDTH, image_size.width);
	cam.setParam(paramTypeCamera::paramInt::HEIGHT, image_size.height);
	cam.setParam(paramTypeCamera::paramFloat::FPS, fps);
	cam.setParam(paramTypeCamera::paramFloat::GAIN, gain);
	cam.setParam(paramTypeBasler::Param::ExposureTime, 10000.0f);
	cam.setParam(paramTypeBasler::AcquisitionMode::EnableAcquisitionFrameRate);
	cam.setParam(paramTypeBasler::FastMode::SensorReadoutModeFast);
	cam.setParam(paramTypeBasler::GrabStrategy::OneByOne);
	//cam.setParam(paramTypeBasler::CaptureType::BayerBGGrab);
	//cam.setParam(paramTypeBasler::CaptureType::ColorBGRGrab);
	cam.setParam(paramTypeBasler::CaptureType::MonocroGrab);
	cam.parameter_all_print();
	cam.start();



}
*/
cv::Mat FaceTracker::get_result() {
	return result;
}


void FaceTracker::face_tracking(cv::Mat img, dlib::shape_predictor sp, dlib::frontal_face_detector detector,float p) {

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	LARGE_INTEGER start, end;



	image = img.clone();
	face_image = image.clone();



	//回転
	
		cv::Mat r = cv::getRotationMatrix2D(nose, image_degree, 1.0);
		cv::warpAffine(face_image, rotation_image, r, img.size(), 0, 1);//what size I should use?
	

	//画像クロップ
	if (faces.size() != 0) {
		roi_origin.x = faces[0].left()+ velonica.width - faces[0].width() * p;
		roi_origin.y = faces[0].top()+ velonica.height- faces[0].height() * p;
		roi_size.width = faces[0].width() + 2 * faces[0].width() * p;
		roi_size.height = faces[0].height() + 2 * faces[0].height() * p;

		if (roi_origin.x < 0) {
			roi_origin.x = 0;
		}
		if (roi_origin.y < 0) {
			roi_origin.y = 0;
		}
		if (roi_origin.x + roi_size.width > image.cols) {
			roi_size.width = image.cols - roi_origin.x;
		}
		if (roi_origin.y + roi_size.height > image.rows) {
			roi_size.height = image.rows - roi_origin.y;
		}


		cv::Rect roi(roi_origin, roi_size);
		face_image = rotation_image(roi);

	}

	//ダウンサンプリング
	if (faces.size() != 0) {
		down_ratio = min(double(faces[0].width()) / 80.0, double(faces[0].height()) / 80.0);
		cv::resize(face_image, face_image, cv::Size(), 1.0 / down_ratio, 1.0 / down_ratio);
	}

	//顔検出
	cv_image<rgb_pixel>  cimg(face_image);
	faces = detector(cimg);

	//顔矩形領域復元
	for (int i = 0; i < faces.size(); i++) {
		faces[0].set_left(roi_origin.x + faces[0].left() * down_ratio);
		faces[0].set_right(roi_origin.x + faces[0].right() * down_ratio);
		faces[0].set_bottom(roi_origin.y + faces[0].bottom() * down_ratio);
		faces[0].set_top(roi_origin.y + faces[0].top() * down_ratio);
	}
	if (faces.size() != 0) {
		face = faces;
	}
	else {
		
		faces = face;
	}

	//顔器官検出
	full_object_detection shape,virtual_shape;
	for (unsigned long i = 0; i < faces.size(); ++i) {
		cv_image<rgb_pixel> c_img(rotation_image);
		shape = sp(c_img, faces[i]);
		
		virtual_shape = shape;
	//顔器官復元
		if (image_degree != 0) {
			for (int i = 0; i < shape.num_parts(); i++) {
				shape.parts[i].x() = std::cos(-degree)*(virtual_shape.parts[i].x() - nose.x) - std::sin(-degree)*(virtual_shape.parts[i].y() - nose.y) + nose.x;
				shape.parts[i].y() = std::sin(-degree)*(virtual_shape.parts[i].x() - nose.x) + std::cos(-degree)*(virtual_shape.parts[i].y() - nose.y) + nose.y;
			}
		}
	//顔矩形復元
		if (image_degree != 0) {
			rect1.x = std::cos(-degree)*(faces[0].left() - nose.x) - std::sin(-degree)*(faces[0].top() - nose.y) + nose.x;
			rect1.y = std::sin(-degree)*(faces[0].left() - nose.x) + std::cos(-degree)*(faces[0].top() - nose.y) + nose.y;
			rect2.x = std::cos(-degree)*(faces[0].left() - nose.x) - std::sin(-degree)*(faces[0].bottom() - nose.y) + nose.x;
			rect2.y = std::sin(-degree)*(faces[0].left() - nose.x) + std::cos(-degree)*(faces[0].bottom() - nose.y) + nose.y;
			rect3.x = std::cos(-degree)*(faces[0].right() - nose.x) - std::sin(-degree)*(faces[0].bottom() - nose.y) + nose.x;
			rect3.y = std::sin(-degree)*(faces[0].right() - nose.x) + std::cos(-degree)*(faces[0].bottom() - nose.y) + nose.y;
			rect4.x = std::cos(-degree)*(faces[0].right() - nose.x) - std::sin(-degree)*(faces[0].top() - nose.y) + nose.x;
			rect4.y = std::sin(-degree)*(faces[0].right() - nose.x) + std::cos(-degree)*(faces[0].top() - nose.y) + nose.y;
		}
		else {
			rect1.x = faces[0].left();
			rect1.y = faces[0].top();
			rect2.x = faces[0].left();
			rect2.y = faces[0].bottom();
			rect3.x = faces[0].right();
			rect3.y = faces[0].bottom();
			rect4.x = faces[0].right();
			rect4.y = faces[0].top();
		}
	//角度抽出
		float dx = virtual_shape.part(16).x() - virtual_shape.part(0).x();
		float dy = virtual_shape.part(16).y() - virtual_shape.part(0).y();

		degree -= atan(dy / dx);
		image_degree += atan(dy / dx)*180.0 / PI;
	
	//描画
		for (int g = 1; g <= shape.num_parts(); g++) {
			cv::circle(image, cv::Point(shape.part(g - 1).x(), shape.part(g - 1).y()), 3, cv::Scalar(0, 0, 200), -1, 1);
		}


		cv::line(image, rect1, rect2, cv::Scalar(0, 255, 0), 3, 16);
		cv::line(image, rect2, rect3, cv::Scalar(0, 255, 0), 3, 16);
		cv::line(image, rect3, rect4, cv::Scalar(0, 255, 0), 3, 16);
		cv::line(image, rect4, rect1, cv::Scalar(0, 255, 0), 3, 16);
	
	//速度抽出
		if (norm(nose) != 0 ) {
			if (abs(shape.part(33).x() - nose.x) < faces[0].width() * 8)velonica.width = shape.part(33).x() - nose.x;
			if (abs(shape.part(33).y() - nose.y) < faces[0].height() * 8)velonica.height = shape.part(33).y() - nose.y;
		}
		nose.x = shape.part(33).x();
		nose.y = shape.part(33).y();
		
	}




	//double time = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
	//std::cout << "time=" << time << endl;
	
	ok = false;
	
	result = image.clone();
	
	ok = true;
}