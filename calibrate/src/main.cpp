#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/aruco/dictionary.hpp"
#include <iostream>
#include <vector>
#include <map>

/**
 * @brief Command line parser keys
 */
const cv::String &keys =
        "{name     |      | diccionari (alfanumèric) }"
        "{params   |      | paràmetres del detector (alfanumèric)}"
        "{rows     |      | files de la matriu ArUco (numèric) }"
        "{cols     |      | columnes de la matriu ArUco (numèric) }"
        "{len      |      | longitud del costat d’una marca ArUco sola, en metres (numèric)}"
        "{dist     |      | distància entre marques ArUco, en metres (numèric)}"
        "{output   |      | nom del fitxer de paràmetres de calibratge en format yaml (alfanumèric)}";


/**
 * @brief Dictionary of aruco markers
 */
std::map<std::string, int> aruco_dict = {
        {"DICT_4X4_50", 0},
        {"DICT_4X4_100", 1},
        {"DICT_4X4_250", 2},
        {"DICT_4X4_1000", 3},
        {"DICT_5X5_50", 4},
        {"DICT_5X5_100", 5},
        {"DICT_5X5_250", 6},
        {"DICT_5X5_1000", 7},
        {"DICT_6X6_50", 8},
        {"DICT_6X6_100", 9},
        {"DICT_6X6_250", 10},
        {"DICT_6X6_1000", 11},
        {"DICT_7X7_50", 12},
        {"DICT_7X7_100", 13},
        {"DICT_7X7_250", 14},
        {"DICT_7X7_1000", 15},
        {"DICT_ARUCO_ORIGINAL", 16},
        {"DICT_APRILTAG_16h5", 17},
        {"DICT_APRILTAG_25h9", 18},
        {"DICT_APRILTAG_36h10", 19},
        {"DICT_APRILTAG_36h11", 20}
};

void calibrateCamera(int calibrationFlags, const cv::Ptr<cv::aruco::GridBoard> &gridBoard,
                     const std::vector<std::vector<std::vector<cv::Point2f>>> &allCorners,
                     const std::vector<std::vector<int>> &allIds, cv::Size &imgSize, cv::Mat &cameraMatrix,
                     cv::Mat &distCoeffs, double &repError);

/**
 * @brief Read camera parameters from file
 * @param filename
 * @param cameraMatrix
 * @param distCoeffs
 * @return success
 */
static bool readDetectorParameters(const std::string& filename, cv::Ptr<cv::aruco::DetectorParameters > &params) {
	cv::FileStorage fs(filename, cv::FileStorage::READ);
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
	fs["perspectiveRemoveIgnoredMarginPerCell"] >> params -> perspectiveRemoveIgnoredMarginPerCell;
	fs["maxErroneousBitsInBorderRate"] >> params -> maxErroneousBitsInBorderRate;
	fs["minOtsuStdDev"] >> params -> minOtsuStdDev;
	fs["errorCorrectionRate"] >> params -> errorCorrectionRate;
	return true;
}

/**
 * @brief Save camera parameters to file
 * @param filename
 * @param imageSize
 * @param aspectRatio
 * @param flags
 * @param cameraMatrix
 * @param distCoeffs
 * @param totalAvgErr
 * @return success
 */
static bool saveCameraParams(const std::string & filename, const cv::Size& imageSize, float aspectRatio,int flags,const cv::Mat & cameraMatrix,const cv::Mat & distCoeffs, double totalAvgErr) {
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;
	time_t tt;
	time(&tt);
	struct tm * t2 = localtime( & tt);
	char buf[1024];
	strftime(buf, sizeof(buf) - 1, "%c", t2);
	fs << "calibration_time" << buf;
	fs << "image_width" << imageSize.width;
	fs << "image_height" << imageSize.height;
	if (flags & cv::CALIB_FIX_ASPECT_RATIO) fs << "aspectRatio" << aspectRatio;
	if (flags != 0) {
		sprintf(buf, "flags: %s%s%s%s",
		flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
		flags & cv::CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
		flags & cv::CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
		flags & cv::CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
	}
	fs << "flags" << flags;
	fs << "camera_matrix" << cameraMatrix;
	fs << "distortion_coefficients" << distCoeffs;
	fs << "avg_reprojection_error" << totalAvgErr;
	return true;
}

/**
 * @brief Main function
 * @param argc
 * @param argv
 */
int main(int argc, char * argv[]) {
    // Command line parser
    cv::CommandLineParser parser(argc, argv, keys);

    // check for valid input
    if (argc < 8) {
        parser.printMessage();
        return -1;
    }

    // Read input parameters
    auto dictionaryName = parser.get<cv::String>("name");
    auto dictionaryId = aruco_dict[dictionaryName];
    auto dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);
    auto params = parser.get<cv::String>("params");
    auto rows = parser.get<int>("rows");
    auto cols = parser.get<int>("cols");
    auto length = parser.get<float>("len");
    auto distance = parser.get<float>("dist");
    auto output = parser.get<cv::String>("output");

    // Declaration of variables for camera calibration
    auto waitTime = 10;
    auto calibrationFlags = 0;
	auto aspectRatio = (float) 1.0;
    auto detectorParams = cv::aruco::DetectorParameters::create();

    // Read detector parameters from file
    bool readOK = readDetectorParameters(params, detectorParams);
	if (!readOK) {
		std::cout << "Invalid detector parameters file" << std::endl;
        return -1;
	}

    // Create grid board
	auto gridBoard = cv::aruco::GridBoard::create(rows, cols, length, distance, dictionary);
	auto board = gridBoard.staticCast<cv::aruco::Board>();

    // Declaration of variables for camera calibration
	std::vector<std::vector<std::vector<cv::Point2f>>> allCorners;
    std::vector<std::vector<cv::Point2f>> corners, rejected;
    std::vector<std::vector<int>> allIds;
    std::vector<cv::Mat> allImgs;
    cv::VideoCapture input;
    std::vector<int> ids;
    input.open(0);
    cv::Size imgSize;

    // Grab images and detect markers
	while (input.grab()) {
		cv::Mat image, imageCopy;
		input.retrieve(image);
		image.copyTo(imageCopy);

		cv::aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);
		if (!ids.empty())
            cv::aruco::drawDetectedMarkers(imageCopy, corners);
		cv::imshow("out", imageCopy);

		char key = (char) cv::waitKey(waitTime);
		if (key == 27)
			break;

		if (key == 'c' && !ids.empty()) {
			std::cout << "\tImage Processed." << std::endl;
			allCorners.push_back(corners);
			allIds.push_back(ids);
			allImgs.push_back(image);
			imgSize = image.size();
		}
	}

	if (allIds.empty()) {
		std::cerr << "Not enough captures for calibration" << std::endl;
		return 0;
	}

    // Calibrate camera
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    double repError;

    calibrateCamera(calibrationFlags, gridBoard, allCorners, allIds, imgSize, cameraMatrix, distCoeffs, repError);

    // Save results
	bool saveOk = saveCameraParams(output, imgSize, aspectRatio, calibrationFlags,cameraMatrix, distCoeffs, repError);
	if (!saveOk) {
        std::cerr << "Cannot save output file" << std::endl;
		return 0;
	}

    // Print results
    std::cout << "Rep Error: " << repError << std::endl;
    std::cout << "Calibration saved to " << output << std::endl;
	return 0;
}

void calibrateCamera(int calibrationFlags, const cv::Ptr<cv::aruco::GridBoard> &gridBoard,const std::vector<std::vector<std::vector<cv::Point2f>>> &allCorners,const std::vector<std::vector<int>> &allIds, cv::Size &imgSize, cv::Mat &cameraMatrix,cv::Mat &distCoeffs, double &repError) {
    // Concatenate all the corners and ids detected
    std::__1::vector < cv::Mat > rvecs, tvecs;
    std::__1::vector<std::__1::vector<cv::Point2f>> allCornersConcatenated;
    std::vector<int> allIdsConcatenated;
    std::vector<int> markerCounterPerFrame;

    // Reserve space for vectors to avoid reallocation
    markerCounterPerFrame.reserve(allCorners.size());
    for (unsigned int i = 0; i < allCorners.size(); i++) {
        markerCounterPerFrame.push_back((int) allCorners[i].size());
        for (unsigned int j = 0; j < allCorners[i].size(); j++) {
            allCornersConcatenated.push_back(allCorners[i][j]);
            allIdsConcatenated.push_back(allIds[i][j]);
        }
    }

    // Calibrate camera using aruco markers
    repError = cv::aruco::calibrateCameraAruco(allCornersConcatenated, allIdsConcatenated,markerCounterPerFrame, gridBoard, imgSize, cameraMatrix, distCoeffs, rvecs, tvecs,calibrationFlags);
}
