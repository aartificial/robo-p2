#include "opencv2/aruco.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/base.hpp"
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
        "{name     |      | Dictionary name in OpenCV predefined dictionaries }"
        "{size     |      | Marker size (in pixels) }"
        "{id       |      | Marker id }"
        "{file     |      | Output file name }";

/**
 * @brief Main function
 * @param argc
 * @param argv
 */
int main(int argc, char* argv[]) {
    cv::CommandLineParser parser(argc, argv, keys);

    if (argc < 4) {
        parser.printMessage();
        return -1;
    }

    auto dictionaryName = parser.get<cv::String>("name");
    auto dictionaryId = arucoDict[dictionaryName];
    auto dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);
    auto markerSize = parser.get<int>("size");
    auto markerId = parser.get<int>("id");
    auto fileName = parser.get<cv::String>("file");

    cv::Mat image, out;
    cv::Scalar value( 255,255,255);
    auto marginSize = 20;

    cv::aruco::drawMarker(dictionary, markerId, markerSize, image, 1);
    copyMakeBorder(image, out, marginSize, marginSize, marginSize, marginSize, cv::BORDER_CONSTANT, value);
    cv::imwrite(fileName, out);
}
