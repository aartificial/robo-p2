#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/aruco.hpp>
#include <vector>
#include <opencv2/aruco/dictionary.hpp>
using namespace cv;
using namespace std;

static bool readDetectorParameters(string filename, Ptr < aruco::DetectorParameters > &params) {
	FileStorage fs(filename, FileStorage::READ);
	if (!fs.isOpened())
		return false;
	fs["adaptiveThreshWinSizeMin"] >> params -> adaptiveThreshWinSizeMin;
	fs["adaptiveThreshWinSizeMax"] >> params -> adaptiveThreshWinSizeMax;
	fs["adaptiveThreshWinSizeStep"] >> params -> adaptiveThreshWinSizeStep;
	fs["adaptiveThreshConstant"] >> params -> adaptiveThreshConstant;
	fs["minMarkerPerimeterRate"] >> params -> minMarkerPerimeterRate;
	fs["maxMarkerPerimeterRate"] >> params -> maxMarkerPerimeterRate;
	fs["polygonalApproxAccuracyRate"] >> params -> polygonalApproxAccuracyRate;
	fs["minCornerDistanceRate"] >> params -> minCornerDistanceRate;
	fs["minDistanceToBorder"] >> params -> minDistanceToBorder;
	fs["minMarkerDistanceRate"] >> params -> minMarkerDistanceRate;
	fs["doCornerRefinement"] >> params -> cornerRefinementMethod;
	fs["cornerRefinementWinSize"] >> params -> cornerRefinementWinSize;
	fs["cornerRefinementMaxIterations"] >> params -> cornerRefinementMaxIterations;
	fs["cornerRefinementMinAccuracy"] >> params -> cornerRefinementMinAccuracy;
	fs["markerBorderBits"] >> params -> markerBorderBits;
	fs["perspectiveRemovePixelPerCell"] >> params -> perspectiveRemovePixelPerCell;
	fs["perspectiveRemoveIgnoredMarginPerCell"] >> params ->
	perspectiveRemoveIgnoredMarginPerCell;
	fs["maxErroneousBitsInBorderRate"] >> params -> maxErroneousBitsInBorderRate;
	fs["minOtsuStdDev"] >> params -> minOtsuStdDev;
	fs["errorCorrectionRate"] >> params -> errorCorrectionRate;
	return true;
}

static bool saveCameraParams(const string & filename, Size imageSize, float aspectRatio,int flags,const Mat & cameraMatrix,const Mat & distCoeffs, double totalAvgErr) {
	FileStorage fs(filename, FileStorage::WRITE);
	if (!fs.isOpened())
		return false;
	time_t tt;
	time( & tt);
	struct tm * t2 = localtime( & tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);
	fs << "calibration_time" << buf;
	fs << "image_width" << imageSize.width;
	fs << "image_height" << imageSize.height;
	if (flags & CALIB_FIX_ASPECT_RATIO) fs << "aspectRatio" << aspectRatio;
	if (flags != 0) {
		sprintf(buf, "flags: %s%s%s%s",
		flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
		flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
		flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
		flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
	}
	fs << "flags" << flags;
	fs << "camera_matrix" << cameraMatrix;
	fs << "distortion_coefficients" << distCoeffs;
	fs << "avg_reprojection_error" << totalAvgErr;
	return true;
}
int main(int argc, char * argv[]) {
	if (argc < 7) {
		std::cerr << "Usage: " << argv[0] << "  <dictionary> <video source no> <squaresX> <squaresY> <squareLength> <markerLength> <fitxerSortida> "  << std::endl;

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

	int squaresX = atoi(argv[3]);

	int squaresY = atoi(argv[4]);

	float squareLength = atof(argv[5]);

	float markerLength = atof(argv[6]);

	String fitxerSortida = argv[7];

	int calibrationFlags = 0;
	float aspectRatio = 1;
	Ptr < aruco::DetectorParameters > detectorParams = aruco::DetectorParameters::create();
	bool readOK = readDetectorParameters(argv[2], detectorParams);
	if (!readOK) {
		cout << "Invalid detector parameters file" << endl;
	}

	Ptr < aruco::GridBoard > gridboard = aruco::GridBoard::create(squaresX, squaresY,
	squareLength, markerLength, dictionary);
	Ptr < aruco::Board > board = gridboard.staticCast < aruco::Board > ();

	vector < vector < vector < Point2f > > > allCorners;
	vector < vector < int > > allIds;
	vector < Mat > allImgs;
	Size imgSize;


	std::vector < int > ids;
	std::vector < std::vector < cv::Point2f >> corners, rejected;
	while (inputVideo.grab()) {
		cv::Mat image, imageCopy;
		inputVideo.retrieve(image);
		image.copyTo(imageCopy);

		cv::aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);

		image.copyTo(imageCopy);
		if (ids.size() > 0) aruco::drawDetectedMarkers(imageCopy, corners);
		imshow("out", imageCopy);
		char key = (char) cv::waitKey(waitTime);
		if (key == 27)
			break;


		if (key == 'c' && ids.size() > 0) {
			cout << "\tImage Processed." << endl;
			allCorners.push_back(corners);
			allIds.push_back(ids);
			allImgs.push_back(image);
			imgSize = image.size();
		}
	}

	if (allIds.size() < 1) {
		cerr << "Not enough captures for calibration" << endl;
		return 0;
	}

	Mat cameraMatrix, distCoeffs;
	vector < Mat > rvecs, tvecs;

	double repError;

	vector < vector < Point2f > > allCornersConcatenated;
	vector < int > allIdsConcatenated;
	vector < int > markerCounterPerFrame;
	markerCounterPerFrame.reserve(allCorners.size());
	for (unsigned int i = 0; i < allCorners.size(); i++) {
		markerCounterPerFrame.push_back((int) allCorners[i].size());
		for (unsigned int j = 0; j < allCorners[i].size(); j++) {
			allCornersConcatenated.push_back(allCorners[i][j]);
			allIdsConcatenated.push_back(allIds[i][j]);
		}
	}

	repError = aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,
	markerCounterPerFrame, gridboard, imgSize, cameraMatrix, distCoeffs, rvecs, tvecs,
	calibrationFlags);

	bool saveOk = saveCameraParams(fitxerSortida, imgSize, aspectRatio, calibrationFlags,
	cameraMatrix, distCoeffs, repError);
	if (!saveOk) {
		cerr << "Cannot save output file" << endl;
		return 0;
	}

	cout << "Rep Error: " << repError << endl;
	cout << "Calibration saved to " << fitxerSortida << endl;
	return 0;
}
