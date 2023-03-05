cmake_minimum_required (VERSION 3.10)

cmake_policy(SET CMP0025 NEW)
cmake_policy(SET CMP0072 NEW)

project (Project)

find_package(Doxygen REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS off)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_ROOT src/)

file(GLOB_RECURSE SRC ./src/ src/*.cpp)

add_executable(ShapeShifter ${SRC})
include_directories(src)
# stupid hack because opengl will have a different relative path structure
# depending on the OS
include_directories(${OPENGL_INCLUDE_DIR}/HEADERS)
include_directories(${OPENGL_INCLUDE_DIR}/GLES3)

target_link_libraries(ShapeShifter Qt5::Gui)
target_link_libraries(ShapeShifter Qt5::Widgets)
target_link_libraries(ShapeShifter Qt5::OpenGL)
target_link_libraries(ShapeShifter ${OPENGL_LIBRARIES})

set_target_properties(ShapeShifter PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

#Doxygen target
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc)
set(DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile)
add_custom_target(doc
                  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc
                  COMMENT "Generating API documentation with Doxygen"
                  VERBATIM)
