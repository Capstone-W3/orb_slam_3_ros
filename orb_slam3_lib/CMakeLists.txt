cmake_minimum_required(VERSION 2.8)
project(orb_slam3_lib)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=native")

# Check for c++11 support
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated ${CMAKE_CXX_FLAGS}")
  add_definitions(-DCOMPILEDWITHC11)
  message(STATUS "Using flag -std=c++11.")
ELSE()
  MESSAGE(ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF()

# From old (maybe not needed due to restructuring)
# LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

find_package(OpenCV 4.0 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 3.0 QUIET)
   if(NOT OpenCV_FOUND)
      find_package(OpenCV 2.4.3 QUIET)
      if(NOT OpenCV_FOUND)
         message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
     endif()
   endif()
endif()

find_package(Eigen3 3.1.0 REQUIRED)
# find_package(Pangolin REQUIRED)

add_subdirectory(Thirdparty/DBoW2)
add_subdirectory(Thirdparty/g2o)

set(LIBS
  ${OpenCV_LIBS}
  ${EIGEN3_LIBS}
  # ${Pangolin_INCLUDE_DIRS}
  DBoW2
  g2o
  # ${PROJECT_SOURCE_DIR}/orb_slam3/Thirdparty/DBoW2/lib/libDBoW2.so
  # ${PROJECT_SOURCE_DIR}/orb_slam3/Thirdparty/g2o/lib/libg2o.so
)

# Need to figure out what to do with this
find_package(catkin REQUIRED)
catkin_package()

# catkin_package (
#   CATKIN_DEPENDS roscpp rospy std_msgs cv_bridge image_transport tf sensor_msgs dynamic_reconfigure message_runtime
#   LIBRARIES {PROJECT_NAME} libDBoW2 libg2o
# )

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/CameraModels
  ${EIGEN3_INCLUDE_DIR}
  # ${Pangolin_INCLUDE_DIRS}
)

# include(${PROJECT_SOURCE_DIR}/orb_slam3/Thirdparty/DBoW2/CMakeLists.txt)
# include(${PROJECT_SOURCE_DIR}/orb_slam3/Thirdparty/g2o/CMakeLists.txt)

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/orb_slam3/lib)


set(SRCS
  src/System.cc
  src/Tracking.cc
  src/LocalMapping.cc
  src/LoopClosing.cc
  src/ORBextractor.cc
  src/ORBmatcher.cc
  src/FrameDrawer.cc
  src/Converter.cc
  src/MapPoint.cc
  src/KeyFrame.cc
  src/Atlas.cc
  src/Map.cc
  # src/MapDrawer.cc
  src/Optimizer.cc
  src/PnPsolver.cc
  src/Frame.cc
  src/KeyFrameDatabase.cc
  src/Sim3Solver.cc
  src/Initializer.cc
  # src/Viewer.cc
  src/ImuTypes.cc
  src/G2oTypes.cc
  src/CameraModels/Pinhole.cpp
  src/CameraModels/KannalaBrandt8.cpp
  src/OptimizableTypes.cpp
  src/MLPnPsolver.cpp
  src/TwoViewReconstruction.cc

  src/IMapPublisher.cc
  src/DefaultImpls.cc
  src/IPublisherThread.cc
)
# add_dependencies (${PROJECT_NAME} g2o DBoW2)

add_library(${PROJECT_NAME} SHARED ${SRCS})
target_link_libraries(${PROJECT_NAME} ${LIBS})


# map serialization addition - library boost serialization 
message(STATUS "Compile With map save/load function")
find_library(BOOST_SERIALIZATION boost_serialization)
if (NOT BOOST_SERIALIZATION)
    message(FATAL_ERROR "Can't find libboost_serialization")
endif()
target_link_libraries(${PROJECT_NAME} ${BOOST_SERIALIZATION})


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)


install(TARGETS ${PROJECT_NAME} DBoW2 g2o
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)


install(DIRECTORY Vocabulary/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/Vocabulary
)



# Build tools - https://github.com/poine/ORB_SLAM2
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
# add_executable(bin_vocabulary
# ${PROJECT_SOURCE_DIR}/src/bin_vocabulary.cc)
# target_link_libraries(bin_vocabulary ${PROJECT_NAME})