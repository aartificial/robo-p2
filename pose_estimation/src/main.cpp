#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <iomanip>
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <vector>
#include <algorithm>
using namespace cv;
using namespace std;

static bool readCameraParameters(string filename, Mat & camMatrix, Mat & distCoeffs) {
	FileStorage fs(filename, FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["camera_matrix"] >> camMatrix;
	fs["distortion_coefficients"] >> distCoeffs;
	return true;
}
int main(int argc, char * argv[]) {
if (argc < 2) {
	std::cerr << "Usage: " << argv[0] << " <video source no.>" << std::endl;
	return -1;
}


String nomDict = argv[1];

int idDiccionari;


if (nomDict == "DICT_4X4_50") {
idDiccionari = 0;
} else if (nomDict == "DICT_4X4_100") {
idDiccionari = 1;
} else if (nomDict == "DICT_4X4_250") {
idDiccionari = 2;
} else if (nomDict == "DICT_4X4_1000") {
idDiccionari = 3;
} else if (nomDict == "DICT_5X5_50") {
idDiccionari = 4;
} else if (nomDict == "DICT_5X5_100") {
idDiccionari = 5;
} else if (nomDict == "DICT_5X5_250") {
idDiccionari = 6;
} else if (nomDict == "DICT_5X5_1000") {
idDiccionari = 7;
} else if (nomDict == "DICT_6X6_50") {
idDiccionari = 8;
} else if (nomDict == "DICT_6X6_100") {
idDiccionari = 9;
} else if (nomDict == "DICT_6X6_250") {
idDiccionari = 10;
} else if (nomDict == "DICT_6X6_1000") {
idDiccionari = 11;
} else if (nomDict == "DICT_7X7_50") {
idDiccionari = 12;
} else if (nomDict == "DICT_7X7_100") {
idDiccionari = 13;
} else if (nomDict == "DICT_7X7_250") {
idDiccionari = 14;
} else if (nomDict == "DICT_8X8_1000") {
idDiccionari = 15;
} else {
idDiccionari = 16;
}

int waitTime = 10;

cv::VideoCapture inputVideo;

inputVideo.open(0);

Ptr < aruco::Dictionary > dictionary = aruco::getPredefinedDictionary(idDiccionari);

Mat camMatrix, distCoeffs;
vector < int > ids;
vector < vector < Point2f > > corners, rejected;
vector < Vec3d > rvecs, tvecs;

bool readOk = readCameraParameters("fitxerCalibracio.yaml", camMatrix, distCoeffs);
if (!readOk) {
	cerr << "Camera configuration .xml NOT OK" << endl;
	return 0;
}

ostringstream vector_to_marker;
while (inputVideo.grab()) {
	cv::Mat image, imageCopy;
	inputVideo.retrieve(image);
	image.copyTo(imageCopy);

	aruco::detectMarkers(image, dictionary, corners, ids);
	if (ids.size() > 0) {

		aruco::estimatePoseSingleMarkers(corners, atof(argv[3]), camMatrix, distCoeffs, rvecs, tvecs);

		aruco::drawDetectedMarkers(imageCopy, corners, ids);

		vector < int > ::iterator itId;
		itId = find(ids.begin(), ids.end(), atoi(argv[2]));
		if (itId != ids.end()) {
			int i = distance(ids.begin(), itId);

			cv::drawFrameAxes(imageCopy, camMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
		}
		vector_to_marker.str(std::string());
		vector_to_marker << std::setprecision(4) << "X: " << std::setw(8) << tvecs[0](0);
		cv::putText(imageCopy, vector_to_marker.str(),
		cv::Point(10, 30), 
		cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(209, 3, 3), 1, cv::LINE_AA); 
		vector_to_marker.str(std::string());
		vector_to_marker.str(std::string());
		vector_to_marker << std::setprecision(4) << "Y: " << std::setw(8) << tvecs[0](1);
		cv::putText(imageCopy, vector_to_marker.str(), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(209, 3, 3), 1, cv::LINE_AA); 
		vector_to_marker.str(std::string());
		vector_to_marker.str(std::string());
		vector_to_marker << std::setprecision(4) << "Z: " << std::setw(8) << tvecs[0](2);
		cv::putText(imageCopy, vector_to_marker.str(),
		cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(209, 3, 3), 1, cv::LINE_AA); 
		vector_to_marker.str(std::string());
	}

	cv::imshow("out", imageCopy);
	char key = (char) cv::waitKey(waitTime);
	if (key == 27)
	break;
	}	
return 0;
}
