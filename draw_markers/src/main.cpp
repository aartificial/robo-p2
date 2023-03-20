#include "opencv2/aruco.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/base.hpp>
#include <map>

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
 * @brief Command line parser keys
 */
const cv::String &keys =
        "{name     |      | Dictionary name in OpenCV predefined dictionaries }";

/**
 * @brief Main function
 * @param argc
 * @param argv
 */
int main(int argc, char* argv[]) {
    // parse command line arguments
    cv::CommandLineParser parser(argc, argv, keys);

    // check for valid input
    if (argc < 2) {
        parser.printMessage();
        return -1;
    }

    // Read input parameters
    auto dictionaryName = parser.get<cv::String>("name");
    auto dictionaryId = arucoDict[dictionaryName];
    auto dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);
    auto waitTime = 10;

    cv::VideoCapture input;
    input.open(0);

    // capture until press ESC or until the end of the video
    while (input.grab()) {
       cv::Mat image, imageCopy;
       input.retrieve(image);
       image.copyTo(imageCopy);

       std::vector<int> ids;
       std::vector<std::vector<cv::Point2f> > corners;

       cv::aruco::detectMarkers(image, dictionary, corners, ids);

       if (!ids.empty())
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);

       cv::imshow("out", imageCopy);
       char key = (char) cv::waitKey(waitTime);
       if (key == 27)
           break;
    }
}
