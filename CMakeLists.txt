cmake_minimum_required (VERSION 2.8.1)

project (Project)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5GUI REQUIRED)
find_package(Qt5OpenGL REQUIRED)

find_package(OpenGL REQUIRED) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_ROOT src/)

file(GLOB SRC src/*.cpp src/ui/*.cpp)
add_executable(ShapeShifter ${SRC})
include_directories(src)

target_link_libraries(ShapeShifter Qt5::Gui)
target_link_libraries(ShapeShifter Qt5::Widgets)
target_link_libraries(ShapeShifter Qt5::OpenGL)
target_link_libraries(ShapeShifter ${OPENGL_LIBRARIES})
