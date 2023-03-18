#include "opencv2/aruco.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/base.hpp>
#include <map>

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

/**
 * @brief Command line parser keys
 */
const cv::String &keys =
        "{name     |      | Dictionary name in OpenCV predefined dictionaries }"
        "{rows     |      | Number of rows }"
        "{cols  |      | Number of columns }"
        "{pixels   |      | Marker size (in pixels) }"
        "{margin   |      | Margins size (in pixels) }"
        "{output   |      | Output file name }";

/**
 * @brief Main function
 * @param argc
 * @param argv
 */
int main(int argc, char* argv[]) {
    // parse command line arguments
    cv::CommandLineParser parser(argc, argv, keys);

    // check for valid input
    if (argc < 5) {
        parser.printMessage();
        return -1;
    }

    // Read input parameters
    auto dictionaryName = parser.get<cv::String>("name");
    auto dictionaryId = aruco_dict[dictionaryName];
    auto dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);
    auto rows = parser.get<int>("rows");
    auto columns = parser.get<int>("cols");
    auto pixels = parser.get<int>("pixels");
    auto margins = parser.get<int>("margin");
    auto output = parser.get<cv::String>("output");

    // Create board
    cv::Size imageSize;
    cv::Mat markerImage;
    imageSize.width = rows *(pixels+margins) - margins + 2 * margins;
    imageSize.height = columns *(pixels+margins) - margins + 2 * margins;
    auto board = cv::aruco::GridBoard::create(columns,rows, float(pixels),float(margins), dictionary);

    // Draw board
    cv::Mat boardImage;
    board->draw(imageSize, boardImage, margins, 2);

    // Save board
    cv::imwrite(output, boardImage);
}
