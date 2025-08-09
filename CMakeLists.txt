cmake_minimum_required(VERSION 3.16)
project(MovieReviewApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

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

target_link_libraries(MovieReviewApp Qt6::Core Qt6::Widgets Qt6::Network)

# For macOS
if(APPLE)
    # Bundle icon into the .app
    set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/AppIcon.icns)
    if(EXISTS ${APP_ICON})
        set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        target_sources(MovieReviewApp PRIVATE ${APP_ICON})
        set_target_properties(MovieReviewApp PROPERTIES
            MACOSX_BUNDLE TRUE
            # CFBundleIconFile expects the file name without extension
            MACOSX_BUNDLE_ICON_FILE "AppIcon"
        )
    else()
        set_target_properties(MovieReviewApp PROPERTIES MACOSX_BUNDLE TRUE)
        message(STATUS "App icon not found at ${APP_ICON}. Build will proceed without a custom icon.")
    endif()
endif()

# For Windows
if(WIN32)
    # Embed app icon and mark as GUI app (no console window)
    set(APP_ICON_WIN ${CMAKE_CURRENT_SOURCE_DIR}/resources/AppIcon.ico)
    if(EXISTS ${APP_ICON_WIN})
        set(APP_ICON_FILE "${APP_ICON_WIN}")
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/resources/AppIcon.rc.in
                       ${CMAKE_CURRENT_BINARY_DIR}/AppIcon.rc @ONLY)
        target_sources(MovieReviewApp PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/AppIcon.rc)
        set_target_properties(MovieReviewApp PROPERTIES WIN32_EXECUTABLE TRUE)
    else()
        message(STATUS "Windows icon not found at ${APP_ICON_WIN}. Build will proceed without a custom icon.")
    endif()
endif()
