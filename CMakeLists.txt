cmake_minimum_required(VERSION 3.13)
project(model-converter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(external/tinyobjloader)
add_subdirectory(external/args)
add_subdirectory(external/glm)
add_subdirectory(external/ZipLib)

add_executable(model-converter
        #sources
        src/main.cpp
        include/Exporter.h
        src/Exporter.cpp

        #headers
)

target_include_directories(model-converter PUBLIC external/tinyobjloader)
target_include_directories(model-converter PUBLIC external/args)
target_include_directories(model-converter PUBLIC external/glm/glm)
target_include_directories(model-converter PUBLIC external/ZipLib)
target_include_directories(model-converter PUBLIC include)

target_link_libraries(model-converter tinyobjloader)
target_link_libraries(model-converter args)
target_link_libraries(model-converter glm)
target_link_libraries(model-converter ZIPLIB)
