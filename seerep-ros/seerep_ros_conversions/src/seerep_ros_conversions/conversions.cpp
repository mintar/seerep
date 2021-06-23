#include "seerep_ros_conversions/conversions.h"

namespace seerep_ros_conversions
{
/*
 * Header
 */
seerep::Header toProto(const std_msgs::Header& header)
{
  seerep::Header ret;
  ret.set_seq(header.seq);
  ret.set_frame_id(header.frame_id);
  ret.mutable_stamp()->set_seconds(header.stamp.sec);
  ret.mutable_stamp()->set_nanos(header.stamp.nsec);
  return ret;
}

std_msgs::Header toROS(const seerep::Header& header)
{
  std_msgs::Header ret;
  ret.seq = header.seq();
  ret.frame_id = header.frame_id();
  ret.stamp.sec = header.stamp().seconds();
  ret.stamp.nsec = header.stamp().nanos();
  return ret;
}

/*
 * PointField
 */
seerep::PointField toProto(const sensor_msgs::PointField& point_field)
{
  seerep::PointField ret;
  ret.set_name(point_field.name);
  ret.set_offset(point_field.offset);
  ret.set_datatype(seerep::PointField_Datatype(point_field.datatype));
  ret.set_count(point_field.count);
  return ret;
}

sensor_msgs::PointField toROS(const seerep::PointField& point_field)
{
  sensor_msgs::PointField ret;
  ret.name = point_field.name();
  ret.offset = point_field.offset();
  ret.datatype = point_field.datatype();
  ret.count = point_field.count();
  return ret;
}

/*
 * PointCloud2
 */
seerep::PointCloud2 toProto(const sensor_msgs::PointCloud2& cloud)
{
  seerep::PointCloud2 ret;
  *ret.mutable_header() = toProto(cloud.header);
  ret.set_height(cloud.height);
  ret.set_width(cloud.width);
  for (auto field : cloud.fields)
    *ret.add_fields() = toProto(field);
  ret.set_is_bigendian(cloud.is_bigendian);
  ret.set_point_step(cloud.point_step);
  ret.set_row_step(cloud.row_step);
  ret.set_data(&cloud.data.front(), cloud.data.size());
  ret.set_is_dense(cloud.is_dense);
  return ret;
}

sensor_msgs::PointCloud2 toROS(const seerep::PointCloud2& cloud)
{
  sensor_msgs::PointCloud2 ret;
  ret.header = toROS(cloud.header());
  ret.height = cloud.height();
  ret.width = cloud.width();
  for (auto field : cloud.fields())
    ret.fields.push_back(toROS(field));
  ret.is_bigendian = cloud.is_bigendian();
  ret.point_step = cloud.point_step();
  ret.row_step = cloud.row_step();
  std::copy(cloud.data().begin(), cloud.data().end(), std::back_inserter(ret.data));
  ret.is_dense = cloud.is_dense();
  return ret;
}

/*
 * Image
 */
seerep::Image toProto(const sensor_msgs::Image& image)
{
  seerep::Image ret;
  *ret.mutable_header() = toProto(image.header);
  ret.set_height(image.height);
  ret.set_width(image.width);
  ret.set_encoding(image.encoding);
  ret.set_is_bigendian(image.is_bigendian);
  ret.set_step(image.step);
  ret.set_data(&image.data.front(), image.data.size());
  return ret;
}

sensor_msgs::Image toROS(const seerep::Image& image)
{
  sensor_msgs::Image ret;
  ret.header = toROS(image.header());
  ret.height = image.height();
  ret.width = image.width();
  ret.encoding = image.encoding();
  ret.is_bigendian = image.is_bigendian();
  ret.step = image.step();
  std::copy(image.data().begin(), image.data().end(), std::back_inserter(ret.data));
  return ret;
}

/*
 * Point
 */
seerep::Point toProto(const geometry_msgs::Point& point)
{
  seerep::Point ret;
  ret.set_x(point.x);
  ret.set_y(point.y);
  ret.set_z(point.z);
  return ret;
}

geometry_msgs::Point toROS(const seerep::Point& point)
{
  geometry_msgs::Point ret;
  ret.x = point.x();
  ret.y = point.y();
  ret.z = point.z();
  return ret;
}

/*
 * Quaternion
 */
seerep::Quaternion toProto(const geometry_msgs::Quaternion& quaternion)
{
  seerep::Quaternion ret;
  ret.set_x(quaternion.x);
  ret.set_y(quaternion.y);
  ret.set_z(quaternion.z);
  ret.set_w(quaternion.w);
  return ret;
}

geometry_msgs::Quaternion toROS(const seerep::Quaternion& quaternion)
{
  geometry_msgs::Quaternion ret;
  ret.x = quaternion.x();
  ret.y = quaternion.y();
  ret.z = quaternion.z();
  ret.w = quaternion.w();
  return ret;
}

/*
 * Pose
 */
seerep::Pose toProto(const geometry_msgs::Pose& pose)
{
  seerep::Pose ret;
  *ret.mutable_position() = toProto(pose.position);
  *ret.mutable_orientation() = toProto(pose.orientation);
  return ret;
}

geometry_msgs::Pose toROS(const seerep::Pose& pose)
{
  geometry_msgs::Pose ret;
  ret.position = toROS(pose.position());
  ret.orientation = toROS(pose.orientation());
  return ret;
}

/*
 * PoseStamped
 */
seerep::PoseStamped toProto(const geometry_msgs::PoseStamped& pose)
{
  seerep::PoseStamped ret;
  *ret.mutable_header() = toProto(pose.header);
  *ret.mutable_pose() = toProto(pose.pose);
  return ret;
}

geometry_msgs::PoseStamped toROS(const seerep::PoseStamped& pose)
{
  geometry_msgs::PoseStamped ret;
  ret.header = toROS(pose.header());
  ret.pose = toROS(pose.pose());
  return ret;
}
} /* namespace seerep_ros_conversions */