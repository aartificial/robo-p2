cmake_minimum_required(VERSION 3.1)
project(robo-p2)

set (CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)

include_directories(${OPENCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

link_directories(${OpenCV_LIBRARY_DIRS})

set(generate_marker_src generate_marker/src/main.cpp)
add_executable(generate_marker ${generate_marker_src})
target_link_libraries(generate_marker ${OpenCV_LIBRARIES})
target_compile_options(generate_marker PRIVATE -O3 -std=c++11)

set(generate_board_src generate_board/src/main.cpp)
add_executable(generate_board ${generate_board_src})
target_link_libraries(generate_board ${OpenCV_LIBRARIES})
target_compile_options(generate_board PRIVATE -O3 -std=c++11)

set(draw_cube_src draw_cube/src/main.cpp)
add_executable(draw_cube ${draw_cube_src})
target_link_libraries(draw_cube ${OpenCV_LIBRARIES})
target_compile_options(draw_cube PRIVATE -O3 -std=c++11)

set(calibrate_src calibrate/src/main.cpp)
add_executable(calibrate ${calibrate_src})
target_link_libraries(calibrate ${OpenCV_LIBRARIES})
target_compile_options(calibrate PRIVATE -O3 -std=c++11)

set(pose_estimation_src pose_estimation/src/main.cpp)
add_executable(pose_estimation ${pose_estimation_src})
target_link_libraries(pose_estimation ${OpenCV_LIBRARIES})
target_compile_options(pose_estimation PRIVATE -O3 -std=c++11)