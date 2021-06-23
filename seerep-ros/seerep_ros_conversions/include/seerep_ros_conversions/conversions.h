#ifndef SEEREP_ROS_CONVERSIONS
#define SEEREP_ROS_CONVERSIONS

// ROS messages
#include <std_msgs/Header.h>
#include <sensor_msgs/PointField.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Image.h>
#include <geometry_msgs/PoseStamped.h>

// Agri-Gaia proto messages
#include <seerep-msgs/header.pb.h>
#include <seerep-msgs/point_field.pb.h>
#include <seerep-msgs/point_cloud_2.pb.h>
#include <seerep-msgs/image.pb.h>
#include <seerep-msgs/pose_stamped.pb.h>

namespace seerep_ros_conversions
{
/**
 * @brief Converts a ROS std_msgs/Header message to the corresponding
 * Protobuf Header message
 * @param header std_msgs/Header
 * @return Protobuf Header message
 */
seerep::Header toProto(const std_msgs::Header& header);

/**
 * @brief Converts a Protobuf Header message to the corresponding
 * ROS std_msgs/Header message
 * @param header Protobuf Header
 * @return ROS std_msgs/Header
 */
std_msgs::Header toROS(const seerep::Header& header);

/**
 * @brief Converts a ROS sensor_msgs/PointField message to the corresponding
 * Protobuf PointField message
 * @param point_field sensor_msgs/PointField
 * @return Protobuf PointField message
 */
seerep::PointField toProto(const sensor_msgs::PointField& point_field);

/**
 * @brief Converts a Protobuf PointField message to the corresponding
 * ROS sensor_msgs/PointField message
 * @param point_field Protobuf PointField
 * @return ROS std_sensor_msgs/PointField
 */
sensor_msgs::PointField toROS(const seerep::PointField& point_field);

/**
 * @brief Converts a ROS sensor_msgs/PointCloud2 message to the corresponding
 * Protobuf PointCloud2 message
 * @param point_field sensor_msgs/PointCloud2
 * @return Protobuf PointCloud2 message
 */
seerep::PointCloud2 toProto(const sensor_msgs::PointCloud2& cloud);

/**
 * @brief Converts a Protobuf PointCloud2 message to the corresponding
 * ROS sensor_msgs/PointCloud2 message
 * @param cloud Protobuf PointCloud2
 * @return ROS std_sensor_msgs/PointCloud2
 */
sensor_msgs::PointCloud2 toROS(const seerep::PointCloud2& cloud);

/**
 * @brief Converts a ROS sensor_msgs/Image message to the corresponding
 * Protobuf Image message
 * @param image sensor_msgs/Image
 * @return Protobuf Image message
 */
seerep::Image toProto(const sensor_msgs::Image& image);

/**
 * @brief Converts a Protobuf Image message to the corresponding
 * ROS sensor_msgs/Image message
 * @param cloud Protobuf Image
 * @return ROS std_sensor_msgs/Image
 */
sensor_msgs::Image toROS(const seerep::Image& image);

/**
 * @brief Converts a ROS geometry_msgs::Point message to the corresponding
 * Protobuf Image message
 * @param point geometry_msgs::Point
 * @return Protobuf Point message
 */
seerep::Point toProto(const geometry_msgs::Point& point);

/**
 * @brief Converts a Protobuf Image message to the corresponding
 * ROS geometry_msgs::Point message
 * @param point Protobuf Point
 * @return ROS geometry_msgs::Point
 */
geometry_msgs::Point toROS(const seerep::Point& point);

/**
 * @brief Converts a ROS geometry_msgs::Quaternion message to the corresponding
 * Protobuf Quaternion message
 * @param quaternion geometry_msgs::Quaternion
 * @return Protobuf Quaternion message
 */
seerep::Quaternion toProto(const geometry_msgs::Quaternion& quaternion);

/**
 * @brief Converts a Protobuf Quaternion message to the corresponding
 * ROS geometry_msgs::Quaternion message
 * @param quaternion Protobuf Quaternion
 * @return ROS geometry_msgs::Quaternion
 */
geometry_msgs::Quaternion toROS(const seerep::Quaternion& quaternion);

/**
 * @brief Converts a ROS geometry_msgs::Pose message to the corresponding
 * Protobuf Pose message
 * @param pose geometry_msgs::Pose
 * @return Protobuf Pose message
 */
seerep::Pose toProto(const geometry_msgs::Pose& pose);

/**
 * @brief Converts a Protobuf Pose message to the corresponding
 * ROS geometry_msgs::Pose message
 * @param pose Protobuf Pose
 * @return ROS geometry_msgs::Pose
 */
geometry_msgs::Pose toROS(const seerep::Pose& pose);

/**
 * @brief Converts a ROS geometry_msgs::PoseStamped message to the corresponding
 * Protobuf PoseStamped message
 * @param pose geometry_msgs::PoseStamped
 * @return Protobuf PoseStamped message
 */
seerep::PoseStamped toProto(const geometry_msgs::PoseStamped& pose);

/**
 * @brief Converts a Protobuf PoseStamped message to the corresponding
 * ROS geometry_msgs::PoseStamped message
 * @param pose Protobuf PoseStamped
 * @return ROS geometry_msgs::PoseStamped
 */
geometry_msgs::PoseStamped toROS(const seerep::PoseStamped& pose);
} /* namespace seerep_ros_conversions */

#endif /* SEEREP_ROS_CONVERSIONS */