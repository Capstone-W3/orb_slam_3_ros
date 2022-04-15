/*
 * ROSSystemBuilder.cpp
 *
 *  Created on: Jun 3, 2018
 *      Author: jarek
 */

#include <include/ROSSystemBuilder.h>

ROSSystemBuilder::ROSSystemBuilder(const std::string& strVocFile,
                const std::string& strSettingsFile,
                ORB_SLAM3::System::eSensor sensor,
                double frequency,
                ros::NodeHandle nh,
                std::string map_frame,
                std::string camera_frame) :
                ORB_SLAM3::System::GenericBuilder(strVocFile, strSettingsFile, sensor)
{

    mpPublisher = make_unique<ROSPublisher>(
        GetMap(), frequency, std::move(nh));
    mpTracker->SetFrameSubscriber(mpPublisher.get());
    mpTracker->SetMapPublisher(mpPublisher.get());

}

// Empty dtor to give a place to the calls to the dtor of unique_ptr members
ROSSystemBuilder::~ROSSystemBuilder() { }

ORB_SLAM3::IPublisherThread* ROSSystemBuilder::GetPublisher()
{
    return mpPublisher.get();
}

