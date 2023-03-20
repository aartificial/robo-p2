#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

namespace {
const cv::String &keys =
    "{name  |      | Dictionary name in OpenCV predefined dictionaries }"
    "{length  |      | Marker side length (in meters). Real world size }"
    "{id  |      | Marker id }";
}

/**
 * @brief Draw axis on image
 * @param imageCopy
 * @param axis
 * @param tvecs
 */
void drawInfo(const cv::Mat &imageCopy, std::ostringstream &axis, const std::vector<cv::Vec3d> &tvecs);

/**
 * @brief Draw a cube with wireframe
 * @param image
 * @param cameraMatrix
 * @param distCoeffs
 * @param rvec
 * @param tvec
 * @param l
 */
void drawCubeWireframe(cv::InputOutputArray image, cv::InputArray cameraMatrix,cv::InputArray distCoeffs, cv::InputArray rvec, cv::InputArray tvec,float l);

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
 * @brief Main function
 * @param argc
 * @param argv
 */
int main(int argc, char **argv) {
    // Parse command line arguments
    cv::CommandLineParser parser(argc, argv, keys);

    // Check for valid input
    if (argc < 3) {
        parser.printMessage();
        return -1;
    }

    // Get command line arguments
    auto dictionaryName = parser.get<cv::String>("name");
    auto length = parser.get<float>("length");
    auto markerId = parser.get<int>("id");
    auto dictionaryId = arucoDict[dictionaryName];
    int waitTime = 10;

    if (length <= 0) {
        std::cerr << "Invalid marker length" << std::endl;
        return -1;
    }

    // Open video input
    cv::VideoCapture input;
    input.open(0);

    // Check if video input is opened
    if (!input.isOpened()) {
        std::cerr << "failed to open video input: " << 0 << std::endl;
        return 1;
    }

    cv::Mat image, imageCopy;
    cv::Mat cameraMatrix, distCoeffs;
    std::ostringstream axis;

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

    // Read calibration file
    cv::FileStorage fs("fitxerCalibracio.yaml", cv::FileStorage::READ);

    fs["cameraMatrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;

    std::cout << "cameraMatrix\n" << cameraMatrix << std::endl;
    std::cout << "\ndist_coeffs\n" << distCoeffs << std::endl;

    while (input.grab()) {
        input.retrieve(image);
        image.copyTo(imageCopy);

        // detect markers
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);

        // if at least one marker detected
        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, length, cameraMatrix, distCoeffs, rvecs, tvecs);

            // draw axis for each marker
            std::vector < int > ::iterator itId;
            auto index = std::find(ids.begin(), ids.end(), markerId);
            if (index != ids.end()) {
                long i = distance(ids.begin(), itId);
                drawCubeWireframe(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], length);
                drawInfo(imageCopy, axis, tvecs);
            }
        }

        cv::imshow("out", imageCopy);
        char key = (char)cv::waitKey(waitTime);
        if (key == 27)
            break;
    }

    input.release();
    return 0;
}

void drawCubeWireframe(cv::InputOutputArray image, cv::InputArray cameraMatrix,cv::InputArray distCoeffs, cv::InputArray rvec, cv::InputArray tvec, float l) {
    // cube half-length
    auto halfLength = (float) (l / 2.0);

    // project cube points
    std::vector<cv::Point3f> axisPoints;
    axisPoints.emplace_back(halfLength, halfLength, l);
    axisPoints.emplace_back(halfLength, -halfLength, l);
    axisPoints.emplace_back(-halfLength, -halfLength, l);
    axisPoints.emplace_back(-halfLength, halfLength, l);
    axisPoints.emplace_back(halfLength, halfLength, 0);
    axisPoints.emplace_back(halfLength, -halfLength, 0);
    axisPoints.emplace_back(-halfLength, -halfLength, 0);
    axisPoints.emplace_back(-halfLength, halfLength, 0);

    // project axis points
    std::vector<cv::Point2f> imagePoints;
    projectPoints(axisPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

    // draw cube edges lines
    cv::line(image, imagePoints[0], imagePoints[1], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[0], imagePoints[3], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[0], imagePoints[4], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[1], imagePoints[2], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[1], imagePoints[5], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[2], imagePoints[3], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[2], imagePoints[6], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[3], imagePoints[7], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[4], imagePoints[5], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[4], imagePoints[7], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[5], imagePoints[6], cv::Scalar(255, 0, 0), 3);
    cv::line(image, imagePoints[6], imagePoints[7], cv::Scalar(255, 0, 0), 3);
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
