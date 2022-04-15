/*
 * ROSSystemBuilder.h
 *
 *  Created on: Jun 3, 2018
 *      Author: jarek
 */

#ifndef SRC_ROSSYSTEMBUILDER_H_
#define SRC_ROSSYSTEMBUILDER_H_

#include "ROSPublisher.h"
#include "utils.h"

const string &strVocFile, ORBParameters& parameters, const eSensor sensor, 
const std::string & map_file = "", bool load_map = false, const bool bUseViewer = true, 
const int initFr = 0, const string &strSequence = std::string(), 
const string &strLoadingFile = std::string())


class ROSSystemBuilder : public ORB_SLAM3::System::GenericBuilder {
public:
    ROSSystemBuilder(const std::string& strVocFile,
                     ORB_SLAM3::Tracking::ORBParameters& parameters,
                     ORB_SLAM3::System::eSensor sensor,
                     double frequency,
                     ros::NodeHandle nh = ros::NodeHandle(),
                     std::string map_frame = ROSPublisher::DEFAULT_MAP_FRAME,
                     std::string camera_frame = ROSPublisher::DEFAULT_CAMERA_FRAME);

    virtual ORB_SLAM3::IPublisherThread* GetPublisher() override;
    virtual ~ROSSystemBuilder();

private:

    std::unique_ptr<ROSPublisher> mpPublisher;
};


#endif /* SRC_ROSSYSTEMBUILDER_H_ */
