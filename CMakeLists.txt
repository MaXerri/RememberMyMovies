cmake_minimum_required(VERSION 3.16)
project(MovieReviewApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/movie.cpp
    src/moviedatabase.cpp
    src/MainWindow.cpp
)

set(HEADERS
    include/movie.h
    include/moviedatabase.h
    include/MainWindow.h
)

add_executable(MovieReviewApp ${SOURCES} ${HEADERS})

target_link_libraries(MovieReviewApp Qt6::Core Qt6::Widgets)

# For macOS
if(APPLE)
    set_target_properties(MovieReviewApp PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()
