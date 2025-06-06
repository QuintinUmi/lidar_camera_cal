#ifndef _CONVERSION_BRIDGE_H_
#define _CONVERSION_BRIDGE_H_

#include <ros/ros.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>

#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <pcl/PCLPointCloud2.h>

#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>  
#include <aruco/aruco.h>  
#include "opencv2/aruco/charuco.hpp"  

#include <Eigen/Dense>
#include <Eigen/SVD>

#include <geometry_msgs/Point.h>
#include <geometry_msgs/Point32.h>

#include <yaml-cpp/yaml.h>

#include "image_transport/image_transport.h"



namespace lcr_cal
{
    class ConversionBridge
    {
        public:

            static Eigen::Vector3f rvecCvToEigen(const cv::Mat& cv_rvec);
            static Eigen::Vector3f tvecCvToEigen(const cv::Mat& cv_tvec);
            static std::vector<Eigen::Vector3f> rvecCvToEigen(const std::vector<cv::Mat>& cv_rvecs);
            static std::vector<Eigen::Vector3f> tvecCvToEigen(const std::vector<cv::Mat>& cv_tvecs);
            static cv::Mat rvecEigenToCv(const Eigen::Vector3f& eg_rvec);
            static cv::Mat tvecEigenToCv(const Eigen::Vector3f& eg_tvec);
            static std::vector<cv::Mat> rvecEigenToCv(const std::vector<Eigen::Vector3f>& eg_rvecs);
            static std::vector<cv::Mat> tvecEigenToCv(const std::vector<Eigen::Vector3f>& eg_tvecs);
            static cv::Mat rvec3dToMat_d(const cv::Vec3d& rvec3d);
            static cv::Mat tvec3dToMat_d(const cv::Vec3d& tvec3d);
            static std::vector<cv::Mat> rvecs3dToMat_d(const std::vector<cv::Vec3d>& rvec3d);
            static std::vector<cv::Mat> tvecs3dToMat_d(const std::vector<cv::Vec3d>& tvec3d);
            static cv::Mat rvec3dToMat_f(const cv::Vec3d& rvec3d);
            static cv::Mat tvec3dToMat_f(const cv::Vec3d& tvec3d);
            static std::vector<cv::Mat> rvecs3dToMat_f(const std::vector<cv::Vec3d>& rvecs3d);
            static std::vector<cv::Mat> tvecs3dToMat_f(const std::vector<cv::Vec3d>& tvecs3d);
            
            static Eigen::Vector3f cv3fToEigen3f(const cv::Vec3f& cv_vec);
            static cv::Vec3f eigen3fToCv3f(const Eigen::Vector3f& eg_vec);

            static cv::Vec3f cv3dToCv3f(const cv::Vec3d& cv_vec);
            static cv::Vec3d cv3fToCv3d(const cv::Vec3f& cv_vec);


            std::vector<cv::Mat> rvecToRmat(const std::vector<cv::Vec3d>& rvec);
            std::vector<cv::Mat> rvecsToRmats(const std::vector<cv::Vec3d>& rvecs);

            static Eigen::Quaterniond rvec3dToQuaternion(const cv::Vec3d& rvec);
            static std::vector<Eigen::Quaterniond> rvecs3dToQuaternions(const std::vector<cv::Vec3d>& rvecs);
            static cv::Vec3d quaternionToRvec3d(const Eigen::Quaterniond& quaternion);
            static std::vector<cv::Vec3d> quaternionsToRvecs3d(const std::vector<Eigen::Quaterniond>& quaternions);

            static geometry_msgs::Point rosPoint32ToPoint(geometry_msgs::Point32 p32);
            static std::vector<geometry_msgs::Point> rosPoint32ToPointMulti(std::vector<geometry_msgs::Point32> p32s);
    };
}


using CBridge = lcr_cal::ConversionBridge;

#endif