cmake_minimum_required(VERSION 3.14)

project(ShareAny LANGUAGES CXX)
#CMAKE_TOOLCHAIN_FILE=/Users/apple/Vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set (wt_DIR /home/len/Lib/wt/build/) #你编译的wt路径
find_package(Boost ${QT_VERSION_MAJOR} COMPONENTS thread filesystem REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Network REQUIRED)
find_package(wt CONFIG REQUIRED)
include_directories( /home/len/Lib/wt/src/ /home/len/Lib/wt/build/)
   #target_link_libraries(main PRIVATE Wt::Wt Wt::HTTP Wt::Test)
find_path(QRENCODE_INCLUDE_DIR NAMES qrencode.h)
find_library(QRENCODE_LIBRARY_RELEASE qrencode)
       #find_library(QRENCODE_LIBRARY_DEBUG qrencoded)
       #set(QRENCODE_LIBRARIES optimized ${QRENCODE_LIBRARY_RELEASE} debug ${QRENCODE_LIBRARY_DEBUG})
       # /home/len/Lib/wt/src/ /home/len/Lib/wt/build/
include_directories(${QRENCODE_INCLUDE_DIR})
       #target_link_libraries(main PRIVATE ${QRENCODE_LIBRARIES})
add_executable(ShareAny
  main.cpp web.cc web.h gui.cc gui.h
)
#/home/len/Lib/wt/build/src/libwt.a /home/len/Lib/wt/build/src/libwttest.so crypto ssl
target_link_libraries(ShareAny  
Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network  Boost::thread Boost::filesystem
${QRENCODE_LIBRARY_RELEASE}   Wt::Wt Wt::HTTP Wt::Test) 
