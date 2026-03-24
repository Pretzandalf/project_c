# cmake_minimum_required(VERSION 3.10)

# project(image_processor)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_BUILD_TYPE Debug)

# set(SOURCE_DIR .)
# set(INCLUDE_DIR .)

# set(SOURCES
#     ${SOURCE_DIR}/ImageProcessor.cpp
#     ${SOURCE_DIR}/Controller.cpp
#     ${SOURCE_DIR}/Image.cpp
#     ${SOURCE_DIR}/BMPReader.cpp
#     ${SOURCE_DIR}/BMPWriter.cpp
#     ${SOURCE_DIR}/Filters.cpp
#     ${SOURCE_DIR}/FiltersMaker.cpp
# )

# set(HEADERS
#     ${INCLUDE_DIR}/Controller.h
#     ${INCLUDE_DIR}/Image.h
#     ${INCLUDE_DIR}/BMPReader.h
#     ${INCLUDE_DIR}/BMPWriter.h
#     ${INCLUDE_DIR}/Filters.h
#     ${INCLUDE_DIR}/FiltersMaker.h
#     ${INCLUDE_DIR}/catch/catch.hpp
# )

# add_executable(image_processor ${SOURCES} ${HEADERS})
# target_include_directories(image_processor PRIVATE ${INCLUDE_DIR})

# # Тесты

# set(TEST_SOURCES
#    ${SOURCE_DIR}/Tests/test.cpp
# )

# add_executable(image_processor_tests ${TEST_SOURCES})
# target_include_directories(image_processor_tests PRIVATE ${INCLUDE_DIR})

# enable_testing()
# add_test(NAME image_processor_tests COMMAND image_processor_tests)




cmake_minimum_required(VERSION 3.10)

project(image_processor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_DIR .)
set(INCLUDE_DIR .)

set(SOURCES
    ${SOURCE_DIR}/ImageProcessor.cpp
    ${SOURCE_DIR}/Controller.cpp
    ${SOURCE_DIR}/Image.cpp
    ${SOURCE_DIR}/BMPReader.cpp
    ${SOURCE_DIR}/BMPWriter.cpp
    ${SOURCE_DIR}/Filters.cpp
    ${SOURCE_DIR}/FiltersMaker.cpp
)

set(HEADERS
    ${INCLUDE_DIR}/Controller.h
    ${INCLUDE_DIR}/Image.h
    ${INCLUDE_DIR}/BMPReader.h
    ${INCLUDE_DIR}/BMPWriter.h
    ${INCLUDE_DIR}/BMPStructures.h
    ${INCLUDE_DIR}/Filters.h
    ${INCLUDE_DIR}/FiltersMaker.h
)

add_executable(image_processor ${SOURCES} ${HEADERS})
target_include_directories(image_processor PRIVATE ${INCLUDE_DIR})