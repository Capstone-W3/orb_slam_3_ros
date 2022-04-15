/**
* This file is part of ORB-SLAM3
*
* Copyright (C) 2017-2020 Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
* Copyright (C) 2014-2016 Raúl Mur-Artal, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
*
* ORB-SLAM3 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM3 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
* the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with ORB-SLAM3.
* If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SYSTEM_H
#define SYSTEM_H

//#define SAVE_TIMES

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <opencv2/core/core.hpp>
#include <sys/resource.h>

#include "Tracking.h"
#include "FrameDrawer.h"
// #include "MapDrawer.h"
#include "Atlas.h"
#include "LocalMapping.h"
#include "LoopClosing.h"
#include "KeyFrameDatabase.h"
#include "ORBVocabulary.h"
// #include "Viewer.h"
#include "ImuTypes.h"

namespace ORB_SLAM3
{

class Verbose
{
public:
    enum eLevel
    {
        VERBOSITY_QUIET=0,
        VERBOSITY_NORMAL=1,
        VERBOSITY_VERBOSE=2,
        VERBOSITY_VERY_VERBOSE=3,
        VERBOSITY_DEBUG=4
    };

    static eLevel th;

public:
    static void PrintMess(std::string str, eLevel lev)
    {
        if(lev <= th)
            cout << str << endl;
    }

    static void SetTh(eLevel _th)
    {
        th = _th;
    }
};

// class Viewer;
class FrameDrawer;
class Atlas;
class Tracking;
class LocalMapping;
class LoopClosing;

struct ORBCameraParameters;
struct ORBMapDrawerParameters;
struct ORBViewerParameters;
struct ORBParameters;

class System
{
public:
    // Input sensor
    enum eSensor{
        MONOCULAR=0,
        STEREO=1,
        RGBD=2,
        IMU_MONOCULAR=3,
        IMU_STEREO=4
    };

    // File type
    enum eFileType{
        TEXT_FILE=0,
        BINARY_FILE=1,
    };

public:


    // A `Builder`'s responsibility is to create, link together and destroy each individual
    // part of a `System`.  The pure virtual methods are all supposed to be simple getters.
    class IBuilder {
    public:
        virtual ~IBuilder();
        virtual eSensor GetSensorType() = 0;
        virtual ORBVocabulary* GetVocabulary() = 0;
        virtual KeyFrameDatabase* GetKeyFrameDatabase() = 0;
        virtual Map* GetMap() = 0;
        virtual Tracking* GetTracker() = 0;
        virtual LocalMapping* GetLocalMapper() = 0;
        virtual LoopClosing* GetLoopCloser() = 0;
        virtual IPublisherThread *GetPublisher() = 0;
    };

    // This `IBuilder` implementation  constructs a system completely with the exception of
    // the IPublisherThread and directly related objects (IFrameSubscriber, IMapPublisher).
    // Since different Builders usually differ only by the class of those publishing objects,
    // it's usually more practical to subclass this class instead of the IBuilder.
    class GenericBuilder : public IBuilder {
    public:
        GenericBuilder(const std::string &strVocFile, const std::string &strSettingsFile, eSensor sensor);
        virtual ~GenericBuilder();

        virtual eSensor GetSensorType() override;
        virtual ORBVocabulary *GetVocabulary() override;
        virtual KeyFrameDatabase *GetKeyFrameDatabase() override;
        virtual Map *GetMap() override;
        virtual Tracking *GetTracker() override;
        virtual LocalMapping *GetLocalMapper() override;
        virtual LoopClosing *GetLoopCloser() override;

    protected:
        eSensor mSensor;
        cv::FileStorage mSettings;
        ORBVocabulary mVocabulary;
        std::unique_ptr<KeyFrameDatabase> mpKeyFrameDatabase;
        std::unique_ptr<Map> mpMap;
        std::unique_ptr<Tracking> mpTracker;
        std::unique_ptr<LocalMapping> mpLocalMapper;
        std::unique_ptr<LoopClosing> mpLoopCloser;
    };

    // Creates the SLAM system.
    // The created `System` object takes ownership of the passed `Builder`.
    // The system will still need to be started by calling `System::Start()`.
    System(std::unique_ptr<IBuilder> builder);



    // Initialize the SLAM system. It launches the Local Mapping, Loop Closing and Viewer threads.
    System(const string &strVocFile, ORBParameters& parameters, const eSensor sensor, const std::string & map_file = "", bool load_map = false, const bool bUseViewer = true, const int initFr = 0, const string &strSequence = std::string(), const string &strLoadingFile = std::string());

/*
    // From ORB-SLAM3
    System(const string &strVocFile, const string &strSettingsFile, const eSensor sensor, 
           const bool bUseViewer = true, const int initFr = 0, const string &strSequence = std::string(), const string &strLoadingFile = std::string());
    // From ORB-SLAM2
        System(const string strVocFile, const eSensor sensor, ORBParameters& parameters,
           const std::string & map_file = "", bool load_map = false); // map serialization addition
*/
    // Proccess the given stereo frame. Images must be synchronized and rectified.
    // Input images: RGB (CV_8UC3) or grayscale (CV_8U). RGB is converted to grayscale.
    // Returns the camera pose (empty if tracking fails).
    cv::Mat TrackStereo(const cv::Mat &imLeft, const cv::Mat &imRight, const double &timestamp, const vector<IMU::Point>& vImuMeas = vector<IMU::Point>(), string filename="");
    // cv::Mat TrackStereo(const cv::Mat &imLeft, const cv::Mat &imRight, const double &timestamp, const vector<IMU::Point>& vImuMeas = vector<IMU::Point>(), const int &seq, string filename="");

    // Process the given rgbd frame. Depthmap must be registered to the RGB frame.
    // Input image: RGB (CV_8UC3) or grayscale (CV_8U). RGB is converted to grayscale.
    // Input depthmap: Float (CV_32F).
    // Returns the camera pose (empty if tracking fails).
    cv::Mat TrackRGBD(const cv::Mat &im, const cv::Mat &depthmap, const double &timestamp, string filename="");
    // cv::Mat TrackRGBD(const cv::Mat &im, const cv::Mat &depthmap, const double &timestamp, const int &seq, string filename="");

    // Proccess the given monocular frame and optionally imu data
    // Input images: RGB (CV_8UC3) or grayscale (CV_8U). RGB is converted to grayscale.
    // Returns the camera pose (empty if tracking fails).
    cv::Mat TrackMonocular(const cv::Mat &im, const double &timestamp, const vector<IMU::Point>& vImuMeas = vector<IMU::Point>(), string filename="");
    // cv::Mat TrackMonocular(const cv::Mat &im, const double &timestamp, const vector<IMU::Point>& vImuMeas = vector<IMU::Point>(), const int &seq, string filename="");

    // This stops local mapping thread (map building) and performs only camera tracking.
    void ActivateLocalizationMode();
    // This resumes local mapping thread and performs SLAM again.
    void DeactivateLocalizationMode();
    
    //Checks the current mode (mapping or localization) and changes the mode if requested
    void EnableLocalizationOnly (bool localize_only);

    // Returns true if there have been a big map change (loop closure, global BA)
    // since last call to this function
    bool MapChanged();

    // Reset the system (clear Atlas or the active map)
    void Reset();
    void ResetActiveMap();

    // All threads will be requested to finish.
    // It waits until all threads have finished.
    // This function must be called before saving the trajectory.
    void Shutdown();

    vector<cv::Mat> GetKeyFrameTrajectory();

    // Save camera trajectory in the TUM RGB-D dataset format.
    // Only for stereo and RGB-D. This method does not work for monocular.
    // Call first Shutdown()
    // See format details at: http://vision.in.tum.de/data/datasets/rgbd-dataset
    void SaveTrajectoryTUM(const string &filename);

    // Save keyframe poses in the TUM RGB-D dataset format.
    // This method works for all sensor input.
    // Call first Shutdown()
    // See format details at: http://vision.in.tum.de/data/datasets/rgbd-dataset
    void SaveKeyFrameTrajectoryTUM(const string &filename);

    void SaveTrajectoryEuRoC(const string &filename);
    void SaveKeyFrameTrajectoryEuRoC(const string &filename);

    // Save data used for initialization debug
    void SaveDebugData(const int &iniIdx);

    // Save camera trajectory in the KITTI dataset format.
    // Only for stereo and RGB-D. This method does not work for monocular.
    // Call first Shutdown()
    // See format details at: http://www.cvlibs.net/datasets/kitti/eval_odometry.php
    void SaveTrajectoryKITTI(const string &filename);

    // TODO: Save/Load functions
    // SaveMap(const string &filename);
    // LoadMap(const string &filename);

    // void SetMinimumKeyFrames (int min_num_kf);

    bool SaveMap(const string &filename);

    // cv::Mat GetCurrentPosition ();

    // Information from most recent processed frame
    // You can call this right after TrackMonocular (or stereo or RGBD)
    int GetTrackingState();
    std::vector<MapPoint*> GetTrackedMapPoints();
    std::vector<cv::KeyPoint> GetTrackedKeyPointsUn();

    cv::Mat DrawCurrentFrame ();

    std::vector<MapPoint*> GetAllMapPoints();

    // For debugging
    double GetTimeFromIMUInit();
    bool isLost();
    bool isFinished();

    void ChangeDataset();

    // For grid mapping
    Atlas* getMap() { return mpAtlas; }
    Tracking* getTracker(){ return mpTracker; }
    LocalMapping* getLocalMapping(){ return mpLocalMapper; }
    LoopClosing* getLoopClosing(){ return mpLoopCloser; }

    //void SaveAtlas(int type);

private:

    bool SetCallStackSize (const rlim_t kNewStackSize);

    rlim_t GetCurrentCallStackSize ();

    bool LoadMap(const string &filename);

    bool currently_localizing_only_;

    bool load_map;

    std::string map_file;

    //bool LoadAtlas(string filename, int type);

    //string CalculateCheckSum(string filename, int type);

    // Input sensor
    eSensor mSensor;

    // ORB vocabulary used for place recognition and feature matching.
    ORBVocabulary* mpVocabulary;

    // KeyFrame database for place recognition (relocalization and loop detection).
    KeyFrameDatabase* mpKeyFrameDatabase;

    // Map structure that stores the pointers to all KeyFrames and MapPoints.
    //Map* mpMap;
    Atlas* mpAtlas;

    // Tracker. It receives a frame and computes the associated camera pose.
    // It also decides when to insert a new keyframe, create some new MapPoints and
    // performs relocalization if tracking fails.
    Tracking* mpTracker;

    // Local Mapper. It manages the local map and performs local bundle adjustment.
    LocalMapping* mpLocalMapper;

    // Loop Closer. It searches loops with every new keyframe. If there is a loop it performs
    // a pose graph optimization and full bundle adjustment (in a new thread) afterwards.
    LoopClosing* mpLoopCloser;

    // The viewer draws the map and the current camera pose. It uses Pangolin.
    // Viewer* mpViewer;

    FrameDrawer* mpFrameDrawer;
    // MapDrawer* mpMapDrawer;

    // System threads: Local Mapping, Loop Closing, Viewer.
    // The Tracking thread "lives" in the main execution thread that creates the System object.
    std::thread* mptLocalMapping;
    std::thread* mptLoopClosing;
    // std::thread* mptViewer;

    // Reset flag
    std::mutex mMutexReset;
    bool mbReset;
    bool mbResetActiveMap;

    // Change mode flags
    std::mutex mMutexMode;
    bool mbActivateLocalizationMode;
    bool mbDeactivateLocalizationMode;

    // Tracking state
    int mTrackingState;
    std::vector<MapPoint*> mTrackedMapPoints;
    std::vector<cv::KeyPoint> mTrackedKeyPointsUn;
    std::mutex mMutexState;

    // // Current position
    // cv::Mat current_position_;
};

}// namespace ORB_SLAM

#endif // SYSTEM_H