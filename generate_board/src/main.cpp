#include <iostream>
#include "opencv2/aruco.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/base.hpp>
#include <map>


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

int main(int argc, char* argv[]) {
    if (argc < 5){
        std::cerr << "Usage: ./generate_board <rows> <columns> <dictionary> <file>" << std::endl;
        std::cerr << "<rows>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        std::cerr << "<columns>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        std::cerr << "<dict>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        std::cerr << "<pixels>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        std::cerr << "<pixelsSeparation>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        std::cerr << "<filename>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        return -1;
    }
    int rows = std::stoi(argv[1]);
    int columns = std::stoi(argv[2]);
    std::string dictionary_name = argv[3];
    int pixels = std::stoi(argv[4]);
    int pixelsSeparation = std::stoi(argv[5]);
    std::string filename = argv[6];
   
    cv::Size imageSize;
   
    imageSize.width = rows *(pixels+pixelsSeparation) - pixelsSeparation + 2 * pixelsSeparation;
    imageSize.height = columns *(pixels+pixelsSeparation) - pixelsSeparation + 2 * pixelsSeparation;

    int dictionaryIndex = aruco_dict[dictionary_name];

    cv::Mat markerImage;
   
    const cv::Ptr<cv::aruco::Dictionary>& dictionary = cv::aruco::getPredefinedDictionary(dictionaryIndex);
 
  cv::Ptr<cv::aruco::GridBoard> board = cv::aruco::GridBoard::create(columns,rows, float(pixels),float(pixelsSeparation), dictionary);
 
  cv::Mat boardImage;
  board->draw(imageSize, boardImage, pixelsSeparation, 2);
 
  cv:imwrite(filename,boardImage);
 
   
}
