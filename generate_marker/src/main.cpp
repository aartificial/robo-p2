#include <iostream>
#include "opencv2/aruco.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <map>

/*
    2.1 Realització del programa: generate_marker
    El programa generarà una única marca ArUco amb un marc d’1 bit d’amplada
    Caldrà passar-li 4 paràmetres:
    1. diccionari (alfanumèric)
    2. id de la marca dins el diccionari (numèric)
    3. mida en píxels de la marca (numèric)
    4. nom del fitxer de la imatge PNG de la marca ArUco (alfanumèric)
    Un exemple de crida per línia de comandes podria ser aquesta:
    build$ ./generate_marker DICT_ARUCO_ORIGINAL 25 200 marker.png
*/

std::map<std::string, int> aruco_dict = {{
      "DICT_6X6_250", 10
}};

int main(int argc, char* argv[]) {
    if (argc < 5){
        std::cerr << "Usage: ./generate_marker <dict> <id> <pixels> <file>" << std::endl;
        std::cerr << "<dict>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        std::cerr << "<id>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        std::cerr << "<pixels>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        std::cerr << "<file>: " << argv[0] << " <dictionary_name source no.>" << std::endl;
        return -1;
    }
    std::string dictionary_name = argv[1];
    int marker_id = std::stoi(argv[2]);
    int marker_pixels = std::stoi(argv[3]);
    std::string filename = argv[4];

    int dictionaryIndex = aruco_dict[dictionary_name];

    cv::Mat markerImage;
    const cv::Ptr<cv::aruco::Dictionary>& dictionary = cv::aruco::getPredefinedDictionary(dictionaryIndex);
    cv::aruco::drawMarker(dictionary, marker_id, marker_pixels, markerImage, 1);
    cv::imwrite(filename, markerImage);
}
