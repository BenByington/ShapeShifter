cmake_minimum_required (VERSION 2.8.1)

project (Project)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)

find_package(OpenGL REQUIRED)

MESSAGE("Opengl include": ${OPENGL_INCLUDE_DIR})
MESSAGE("Found Opengl:" ${OPENGL_FOUND})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_ROOT src/)

file(GLOB_RECURSE SRC ./src/ src/*.cpp)

add_executable(ShapeShifter ${SRC})
include_directories(src)
# stupid hack because opengl will have a different relative path structure
# depending on the OS
include_directories(${OPENGL_INCLUDE_DIR}/opengl)
include_directories(${OPENGL_INCLUDE_DIR}/GLES3)

target_link_libraries(ShapeShifter Qt5::Gui)
target_link_libraries(ShapeShifter Qt5::Widgets)
target_link_libraries(ShapeShifter Qt5::OpenGL)
target_link_libraries(ShapeShifter ${OPENGL_LIBRARIES})
