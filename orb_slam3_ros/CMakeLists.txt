cmake_minimum_required(VERSION 2.8)
project(orb_slam3_ros)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -O3 -march=native")

ADD_DEFINITIONS("-std=c++11")

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

find_package(catkin REQUIRED COMPONENTS
  orb_slam3_lib
  roscpp
  tf
  std_msgs
  sensor_msgs
  image_transport
  image_geometry
  cv_bridge
  pcl_ros
  pcl_conversions
  message_generation
  octomap_ros
  dynamic_reconfigure
)

# From new
# find_package (catkin REQUIRED COMPONENTS
# roscpp
# rospy
# std_msgs
# cv_bridge
# image_transport
# tf
# tf2_geometry_msgs
# tf2_ros
# sensor_msgs
# dynamic_reconfigure
# message_generation
# )

# catkin_package (
#   CATKIN_DEPENDS roscpp rospy std_msgs cv_bridge image_transport tf sensor_msgs dynamic_reconfigure message_runtime
#   LIBRARIES {PROJECT_NAME} libDBoW2 libg2o
# )

set (DYNAMIC_RECONFIGURE_PATH config/dynamic_reconfigure.cfg)
execute_process(COMMAND chmod a+x ${DYNAMIC_RECONFIGURE_PATH}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE cmd_result
        OUTPUT_VARIABLE cmd_ver)
message(STATUS "Chmod a+x the dynamic_reconfigure file")

generate_dynamic_reconfigure_options(
        ${DYNAMIC_RECONFIGURE_PATH}
)

add_service_files(
  FILES
  SaveMap.srv
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

# Generate messages in the 'msg' folder
add_message_files(
  FILES
  ORBState.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
  INCLUDE_DIRS include
  #LIBRARIES 
  CATKIN_DEPENDS message_runtime std_msgs
  #DEPENDS 
)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${roscpp_INCLUDE_DIRS}
  ${octomap_ros_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/../orb_slam3_lib
  ${PROJECT_SOURCE_DIR}/../orb_slam3_lib/include
)

add_library(PublisherUtils
 ${PROJECT_SOURCE_DIR}/include/PublisherUtils.h
 ${PROJECT_SOURCE_DIR}/src/PublisherUtils.cpp
 ${PROJECT_SOURCE_DIR}/include/PublisherUtils_impl.h
)

add_dependencies(PublisherUtils ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_library(ScaleCorrector
 ${PROJECT_SOURCE_DIR}/include/ScaleCorrector.h
 ${PROJECT_SOURCE_DIR}/src/ScaleCorrector.cpp
)

add_dependencies(ScaleCorrector ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

add_library(ROSSystemBuilder
 ${PROJECT_SOURCE_DIR}/include/ROSSystemBuilder.h
 ${PROJECT_SOURCE_DIR}/src/ROSSystemBuilder.cpp
)

add_dependencies(ROSSystemBuilder ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

set(LIBS
  ${roscpp_LIBRARIES}
  ${OpenCV_LIBS}
  ${catkin_LIBRARIES}
  ${octomap_ros_LIBRARIES}
  orb_slam3_lib
  PublisherUtils
  ScaleCorrector
  ROSSystemBuilder
)

# Node for monocular camera
add_executable(${PROJECT_NAME}_mono
  src/ros_mono.cc
  src/ROSPublisher.cpp
)

add_dependencies(${PROJECT_NAME}_mono ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

target_link_libraries(${PROJECT_NAME}_mono
  ${LIBS}
)

# Node for stereo camera
add_executable(${PROJECT_NAME}_stereo
  src/ros_stereo.cc
  src/ROSPublisher.cpp
)

add_dependencies(${PROJECT_NAME}_stereo ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(${PROJECT_NAME}_stereo
  ${LIBS}
)

# Node for RGB-D camera
add_executable(${PROJECT_NAME}_rgbd
  src/ros_rgbd.cc
  src/ROSPublisher.cpp
)

add_dependencies(${PROJECT_NAME}_rgbd ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(${PROJECT_NAME}_rgbd
  ${LIBS}
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)

install(DIRECTORY config/
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/config
)
