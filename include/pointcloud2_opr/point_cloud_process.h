#ifndef _POINT_CLOUD_PROCESS_H_
#define _POINT_CLOUD_PROCESS_H_

#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/conversions.h>

#include <pcl/search/impl/search.hpp>
#include <pcl/search/kdtree.h>
#include <pcl/features/normal_3d.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/features/feature.h>
#include <pcl/common/common.h>
#include <pcl/common/pca.h>
#include <pcl/common/transforms.h>
#include <pcl/surface/concave_hull.h>

#include <opencv2/opencv.hpp>

#include <Eigen/Core>

#include <pcl/visualization/cloud_viewer.h> 


namespace lcr_cal {

    namespace pointcloud2_opr {

        template<typename PointT>
        class PointCloud2Proc
        {
        public:
            enum class OptimizationMethod
            {
                None,
                AdjustCentroid,
                AngleAtCentroid
            };

        public:
            PointCloud2Proc(bool remove_origin_point=false);
            PointCloud2Proc(typename pcl::PointCloud<PointT>::Ptr cloud, bool remove_origin_point=false);
            ~PointCloud2Proc();

            int loadPointCloudFile(std::string file_name);

            void resetCloud();
            void setCloud(typename pcl::PointCloud<PointT>::Ptr cloud);
            void setRemoveOriginPoint(bool remove_origin_point=true);

            typename pcl::PointCloud<PointT>::Ptr getRawPointcloud();
            typename pcl::PointCloud<PointT>::Ptr getProcessedPointcloud();
            std::vector<pcl::PointIndices> getClastersIndices();
            pcl::PointIndices getClusterIndices(int index_of_cluster_indices);
            pcl::ModelCoefficients getPlaneCoefficients();
            Eigen::Vector3f getPlaneNormals();
            Eigen::Matrix4f getPCATransformMatrix();
            cv::Point2f* getPCAPlaneRectCorners();
            typename pcl::PointCloud<PointT>::Ptr get3DRectCorners();

            typename pcl::PointCloud<pcl::PointXYZRGB>::Ptr getColorPointCloudIntensity();

            void PassThroughFilter(std::string axis, float min, float max);
            void boxFilter(Eigen::Vector4f min_point, Eigen::Vector4f max_point, bool negative=false);
            void boxFilter(Eigen::Vector3f box_center, float length_x, float length_y, float length_z,
                            float angle_x=0.0, float angle_y=0.0, float angle_z=0.0, bool negative=false);
            std::vector<pcl::PointIndices> normalClusterExtraction(float smoothness = 20.0 / 180.0 * M_PI, float curvature = 0.8, int k_search = 70, 
                                                    int number_of_neighbours = 30, int min_cluster_size = 150, int max_cluster_size = 25000);
            std::vector<pcl::PointIndices> normalClusterExtraction(int (*ClustersIndexSelectorFunction)(std::vector<pcl::PointIndices>), 
                                                    float smoothness = 3.0 / 180.0 * M_PI, float curvature = 0.05, int k_search = 90, 
                                                    int number_of_neighbours = 30, int min_cluster_size = 100, int max_cluster_size = 25000);
            pcl::PointIndices extractNearestClusterCloud(Eigen::Vector4f referencePoint = Eigen::Vector4f(0.0, 0.0, 0.0, 0.0));    
            int statisticalOutlierFilter(int mean_k=50, float stddev_mul=1.0);
            pcl::PointIndices planeSegmentation(float distance_threshold = 0.1, int max_iterations = 1000);
            void planeProjection();
            void planeProjection(pcl::ModelCoefficients::Ptr plane_coefficients);
            void planeProjection(Eigen::Vector4f coefficient);
            void pcaTransform();
            void scaleTo(float scale);
            void transform(Eigen::Matrix4f transform_matrix);
            void transform(Eigen::Matrix3f rotation_matrix, Eigen::Vector3f translation_matrix);
            void transform(Eigen::Matrix3f rotation_matrix, Eigen::Vector4f translation_matrix);
            void transformWorldToImg();
            void transformImgToWorld();
            void findRectangleCornersInPCAPlane();
            void optimizeRectangleAdjustCentroid(float constraint_width, float constraint_height, float optimize_offset_ratio = 0.05, float optimize_precision = 0.001);
            void optimizeRectangleAngleAtCentroid(float constraint_width, float constraint_height, float angle_range_ratio = 0.05, float angle_precision = 0.001);
            void transformCornersTo3D();
            void transformCornersTo3D(Eigen::Matrix4f transform_matrix);
            void extractConcaveHull(double alpha=0.1);
            void extractConvexHull();

            typename pcl::PointCloud<PointT>::Ptr extractNearestRectangleCorners(bool useStatisticalOutlierFilter=false, OptimizationMethod optimization_method=OptimizationMethod::AngleAtCentroid,
                                                                                float constraint_width = 0.0, float constraint_height = 0.0, 
                                                                                float optimize_range_ratio = (0.05F), float optimize_precision = (0.001F));

            typename pcl::PointCloud<PointT>::Ptr getFilterBoxCorners(Eigen::Vector4f min_point, Eigen::Vector4f max_point);
            typename pcl::PointCloud<PointT>::Ptr getFilterBoxCorners(Eigen::Vector3f box_center, float length_x, float length_y, float length_z,
                                                                        float angle_x=0.0, float angle_y=0.0, float angle_z=0.0);
            pcl::PointCloud<pcl::Normal>::Ptr computeNormals(int k_search=120);
            pcl::PointIndices computeNearestClusterIndices(typename pcl::PointCloud<PointT>::Ptr cloud, std::vector<pcl::PointIndices> clusters, Eigen::Vector4f referencePoint = Eigen::Vector4f(0.0, 0.0, 0.0, 0.0));
            void computePCAMatrix(Eigen::Matrix3f& eigen_vector, Eigen::Vector4f& mean_vector);
            Eigen::Matrix4f computeTransformMatrix(Eigen::Matrix3f rotation_matrix, Eigen::Vector4f translation_matrix);
            Eigen::Matrix4f computeTransformMatrixWorldToImg();
            Eigen::Matrix4f computeTransformMatrixImgToWorld();
            void sortPointByNormal(typename pcl::PointCloud<PointT>::Ptr points, const Eigen::Vector3f& normal);

            typename pcl::PointCloud<PointT>::Ptr calculateConcaveHull(const typename pcl::PointCloud<PointT>::Ptr &input_cloud, double alpha=0.1);
            typename pcl::PointCloud<PointT>::Ptr calculateConvexHull(const typename pcl::PointCloud<PointT>::Ptr &input_cloud);

        
            void rawCloudUpdate(typename pcl::PointCloud<PointT>::Ptr cloud);
            void processedCloudUpdate(typename pcl::PointCloud<PointT>::Ptr cloud);
            void processedCloudUpdate(typename pcl::PointCloud<PointT>::Ptr cloud, pcl::PointIndices cluster_indices);
            void processedCloudUpdate(typename pcl::PointCloud<PointT>::Ptr cloud, std::vector<pcl::PointIndices> clusters_indices);
            void processedCloudUpdate(typename pcl::PointCloud<PointT>::Ptr cloud, pcl::Indices indices);

            void processedCloudTransform(typename pcl::PointCloud<PointT>::Ptr cloud, Eigen::Matrix4f transform_matrix);

            std::vector<std::string> iterateFilesFromPath(std::string folderPath);
            void removeOriginPoint();

            typename pcl::PointCloud<PointT>::Ptr NOCLOUD();

        private:
            typename pcl::PointCloud<PointT>::Ptr raw_cloud;
            typename pcl::PointCloud<PointT>::Ptr processed_cloud;

            pcl::PointCloud<pcl::Normal>::Ptr normals;
            std::vector<pcl::PointIndices> clusters;

            pcl::ModelCoefficients::Ptr plane_coefficients;
            Eigen::Vector3f plane_normals;
            Eigen::Matrix4f pca_transform_matrix;
            std::vector<cv::Point2f> pca_points_;
            cv::RotatedRect pointcloud_rect_box;
            cv::Point2f rect_corners_2d[4];
            typename pcl::PointCloud<PointT>::Ptr rect_corners_3d;

            typename pcl::PointCloud<PointT>::Ptr no_cloud;


            typename pcl::search::Search<PointT>::Ptr tree;

        private:
            bool remove_origin_point_;
            int k_search_;
            typename pcl::search::Search<PointT>::Ptr search_method_;

            bool isPointInQuad(const cv::Point2f testCorners[4], const cv::Point2f& P) 
            {
                return isPointInTriangle(testCorners[0], testCorners[1], testCorners[2], P) || isPointInTriangle(testCorners[2], testCorners[3], testCorners[0], P);
            }

            bool isPointInTriangle(const cv::Point2f& A, const cv::Point2f& B, const cv::Point2f& C, const cv::Point2f& P) 
            {
                cv::Point2f AB = B - A;
                cv::Point2f BC = C - B;
                cv::Point2f CA = A - C;

                cv::Point2f AP = P - A;
                cv::Point2f BP = P - B;
                cv::Point2f CP = P - C;

                float cross1 = AB.x * AP.y - AB.y * AP.x;
                float cross2 = BC.x * BP.y - BC.y * BP.x;
                float cross3 = CA.x * CP.y - CA.y * CP.x;

                bool has_neg = (cross1 < 0) || (cross2 < 0) || (cross3 < 0);
                bool has_pos = (cross1 > 0) || (cross2 > 0) || (cross3 > 0);

                return !(has_neg && has_pos);
            }

        };



    }

}




#include "../lib/pointcloud2_opr/point_cloud_process.tpp"


#endif