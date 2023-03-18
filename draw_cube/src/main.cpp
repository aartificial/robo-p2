#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>


namespace {
const char* about = "Draw cube on ArUco marker images";
const cv::String &keys =
    "{@name  |      | Dictionary name in OpenCV predefined dictionaries }"
    "{@length  |      | Marker side length (in meters). Real world size }"
    "{@id  |      | Marker id }";
}

void drawCubeWireframe(
    cv::InputOutputArray image, cv::InputArray cameraMatrix,
    cv::InputArray distCoeffs, cv::InputArray rvec, cv::InputArray tvec,
    float l
);


int main(int argc, char **argv)
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);

    if (argc < 2) {
        parser.printMessage();
        return 1;
    }

  
    
    float marker_length_m = atof(argv[3]);
    int id_marker = atoi(argv[2]);
    int wait_time = 10;

    if (marker_length_m <= 0) {
        std::cerr << "marker length must be a positive value in meter" 
                  << std::endl;
        return 1;
    }

	   
	cv::String nomDict = argv[1];

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

    cv::String videoInput = "0";
    cv::VideoCapture in_video;
    
        in_video.open(0);
    

    if (!parser.check()) {
        parser.printErrors();
        return 1;
    }

    if (!in_video.isOpened()) {
        std::cerr << "failed to open video input: " << videoInput << std::endl;
        return 1;
    }

    cv::Mat image, image_copy;
    cv::Mat camera_matrix, dist_coeffs;
    std::ostringstream vector_to_marker;

    cv::Ptr<cv::aruco::Dictionary> dictionary =    cv::aruco::getPredefinedDictionary( \
        cv::aruco::PREDEFINED_DICTIONARY_NAME(idDiccionari));

    cv::FileStorage fs("fitxerCalibracio.yaml", cv::FileStorage::READ);

    fs["camera_matrix"] >> camera_matrix;
    fs["distortion_coefficients"] >> dist_coeffs;

    std::cout << "camera_matrix\n"
              << camera_matrix << std::endl;
    std::cout << "\ndist coeffs\n"
              << dist_coeffs << std::endl;

    int frame_width = in_video.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = in_video.get(cv::CAP_PROP_FRAME_HEIGHT);
    int fps = 30;
    int fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    cv::VideoWriter video(
        "out.avi", fourcc, fps, cv::Size(frame_width, frame_height), true
    );

    while (in_video.grab())
    {

        in_video.retrieve(image);
        image.copyTo(image_copy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);

        // if at least one marker detected
         if (ids.size() > 0)
        {
            cv::aruco::drawDetectedMarkers(image_copy, corners, ids);
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(
                corners, marker_length_m, camera_matrix, dist_coeffs,
                rvecs, tvecs
            );

            // draw axis for each marker
            
            
            	std::vector < int > ::iterator itId;
			itId = find(ids.begin(), ids.end(), atoi(argv[2]));
			if (itId != ids.end()) {
			int i = distance(ids.begin(), itId);

			drawCubeWireframe(
                   	 image_copy, camera_matrix, dist_coeffs, rvecs[i], tvecs[i],
                   	 marker_length_m
                	);
		}
                
                


                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4)
                                 << "x: " << std::setw(8) << tvecs[0](0);
                cv::putText(image_copy, vector_to_marker.str(),
                            cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                            cv::Scalar(0, 252, 124), 1, CV_AVX);

                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4)
                                 << "y: " << std::setw(8) << tvecs[0](1);
                cv::putText(image_copy, vector_to_marker.str(),
                            cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                            cv::Scalar(0, 252, 124), 1, CV_AVX);

                vector_to_marker.str(std::string());
                vector_to_marker << std::setprecision(4)
                                 << "z: " << std::setw(8) << tvecs[0](2);
                cv::putText(image_copy, vector_to_marker.str(),
                            cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                            cv::Scalar(0, 252, 124), 1, CV_AVX);
            
        }

        video.write(image_copy);
        cv::imshow("Pose estimation", image_copy);
        char key = (char)cv::waitKey(wait_time);
        if (key == 27)
            break;
    }

    in_video.release();

    return 0;
}

void drawCubeWireframe(
    cv::InputOutputArray image, cv::InputArray cameraMatrix,
    cv::InputArray distCoeffs, cv::InputArray rvec, cv::InputArray tvec,
    float l
)
{

    CV_Assert(
        image.getMat().total() != 0 &&
        (image.getMat().channels() == 1 || image.getMat().channels() == 3)
    );
    CV_Assert(l > 0);
    float half_l = l / 2.0;

    // project cube points
    std::vector<cv::Point3f> axisPoints;
    axisPoints.push_back(cv::Point3f(half_l, half_l, l));
    axisPoints.push_back(cv::Point3f(half_l, -half_l, l));
    axisPoints.push_back(cv::Point3f(-half_l, -half_l, l));
    axisPoints.push_back(cv::Point3f(-half_l, half_l, l));
    axisPoints.push_back(cv::Point3f(half_l, half_l, 0));
    axisPoints.push_back(cv::Point3f(half_l, -half_l, 0));
    axisPoints.push_back(cv::Point3f(-half_l, -half_l, 0));
    axisPoints.push_back(cv::Point3f(-half_l, half_l, 0));

    std::vector<cv::Point2f> imagePoints;
    projectPoints(
        axisPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints
    );

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
