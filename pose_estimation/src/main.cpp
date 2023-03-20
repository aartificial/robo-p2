#include <iostream>
#include <opencv2/core/cvstd.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/aruco/dictionary.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <iomanip>
#include <opencv2/imgproc.hpp>
#include <map>

/**
 * @brief Command line parser keys
 */
const cv::String &keys =
    "{name     |      | Dictionary name in OpenCV predefined dictionaries }"
    "{length   |      | Marker side length (in meters). Real world size }"
    "{id       |      | Marker id }";

/**
 * @brief Dictionary of aruco markers
 */
std::map<std::string, int> arucoDict = {
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

/**
 * @brief Draw axis on image
 * @param imageCopy
 * @param axis
 * @param tvecs
 */
void drawInfo(const cv::Mat &imageCopy, std::ostringstream &axis, const std::vector<cv::Vec3d> &tvecs);

/**
 * @brief Main function
 * @param argc
 * @param argv
 */
int main(int argc, char **argv) {
    // parse command line arguments
    cv::CommandLineParser parser(argc, argv, keys);

    // check for valid input
    if (argc < 3) {
        parser.printMessage();
        return -1;
    }

    std::cout << "name: " << parser.get<std::string>("name") << std::endl;
    std::cout << "id: " << parser.get<int>("id") << std::endl;
    std::cout << "length: " << parser.get<float>("length") << std::endl;

    auto dictionaryName = parser.get<cv::String>("name");
    auto markerId = parser.get<int>("id");
    auto length = parser.get<float>("length");
    auto waitTime = 10;
    auto dictionaryId = arucoDict[dictionaryName];

    if (length <= 0) {
        std::cerr << "Invalid marker length" << std::endl;
        return -1;
    }

    // open video file or capturing device
    cv::VideoCapture input;
    input.open(0);

    if (!input.isOpened()) {
        std::cerr << "Could not open video" << std::endl;
        return -1;
    }

    cv::Mat image, imageCopy;
    cv::Mat cameraMatrix, distCoeffs;
    std::ostringstream axis;

    // create aruco dictionary
    auto dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);

    // read camera parameters
    cv::FileStorage fs("fitxerCalibracio.yaml", cv::FileStorage::READ);
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;

    // print camera parameters
    std::cout << "cameraMatrix: " << cameraMatrix << std::endl;
    std::cout << "distCoeffs: " << distCoeffs << std::endl;

    // capture until press ESC or until the end of the video
    while (input.grab()) {
        input.retrieve(image);
        image.copyTo(imageCopy);

        // variables to be filled by detectMarkers and estimatePoseSingleMarkers
        std::vector<int> ids; // ids of the detected markers
        std::vector<std::vector<cv::Point2f> > corners, rejected; // corners of the detected markers, rejected candidates
        std::vector<cv::Vec3d> rvecs, tvecs; // rotation and translation vectors

        // detect markers and estimate pose
        cv::aruco::detectMarkers(image, dictionary, corners, ids, cv::aruco::DetectorParameters::create(), rejected);
        if (!ids.empty())
            cv::aruco::estimatePoseSingleMarkers(corners, length, cameraMatrix, distCoeffs, rvecs, tvecs);

        // draw axis for marker
        if (!ids.empty()) {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
            auto index = std::find(ids.begin(), ids.end(), markerId);
            if (index != ids.end()) {
                long i = std::distance(ids.begin(), index);
                cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], length * 0.5f);
                drawInfo(imageCopy, axis, tvecs);
            }
        }

        // draw image with axis and info
        cv::imshow("out", imageCopy);
        char key = (char) cv::waitKey(waitTime);
        if (key == 27)
            break;
    }

    // close video file or capturing device
    input.release();
    return 0;
}

void drawInfo(const cv::Mat &imageCopy, std::ostringstream &axis, const std::vector<cv::Vec3d> &tvecs) {
    axis.str(std::string());
    axis << std::setprecision(4) << "x: " << std::setw(8) << tvecs[0](0);
    cv::putText(imageCopy, axis.str(),cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6,cv::Scalar(0, 252, 124), 1, CV_AVX);

    axis.str(std::string());
    axis << std::setprecision(4) << "y: " << std::setw(8) << tvecs[0](1);
    cv::putText(imageCopy, axis.str(),cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.6,cv::Scalar(0, 252, 124), 1, CV_AVX);

    axis.str(std::string());
    axis << std::setprecision(4) << "z: " << std::setw(8) << tvecs[0](2);
    cv::putText(imageCopy, axis.str(),cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 0.6,cv::Scalar(0, 252, 124), 1, CV_AVX);
}
