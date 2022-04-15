#include "ros_node_mono.h"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "node_mono");
    ros::start();

    if(argc > 1) {
        ROS_WARN ("Arguments supplied via command line are neglected.");
    }

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ros::NodeHandle node_handle;
    image_transport::ImageTransport image_transport (node_handle);

    MonoNode node (ORB_SLAM3::System::MONOCULAR, node_handle, image_transport);

    node.Init();
    
    node.Start();
    ros::spin();

    node.Shutdown();

    ros::shutdown();

    return 0;
}


MonoNode::MonoNode (ORB_SLAM3::System::eSensor sensor, ros::NodeHandle &node_handle, 
                    image_transport::ImageTransport &image_transport) 
                    : Node (sensor, node_handle, image_transport) {

    std::string image_topic, camera_info;

    node_handle.param<std::string>("/orb_slam3_ros/topic/image_topic", image_topic, ROSPublisher::DEFAULT_IMAGE_TOPIC);
    node_handle.param<std::string>("/orb_slam3_ros/topic/camera_info", camera_info);
    
    image_subscriber = image_transport.subscribe (image_topic, 1, &MonoNode::ImageCallback, this);
    camera_info_topic_ = camera_info;
}


MonoNode::~MonoNode () {
}


void MonoNode::ImageCallback (const sensor_msgs::ImageConstPtr& msg) {
  cv_bridge::CvImageConstPtr cv_in_ptr;
  try {
      cv_in_ptr = cv_bridge::toCvShare(msg);
  } catch (cv_bridge::Exception& e) {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
  }

  current_frame_time_ = msg->header.stamp;

  orb_slam_->TrackMonocular(cv_in_ptr->image,cv_in_ptr->header.stamp.toSec());

//   cv::Mat position = orb_slam_->TrackMonocular(cv_in_ptr->image,cv_in_ptr->header.stamp.toSec());

}
