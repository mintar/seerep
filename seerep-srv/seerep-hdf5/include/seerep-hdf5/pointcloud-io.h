#ifndef SEEREP_HDF5_IO_POINT_CLOUD_H_
#define SEEREP_HDF5_IO_POINT_CLOUD_H_

// highfive
#include <highfive/H5File.hpp>

// seerep-io
#include "seerep-hdf5/general-io.h"

// seerep-msgs
#include <seerep-msgs/point_cloud_2.pb.h>

// std
#include <optional>

#include <boost/geometry.hpp>

namespace seerep_hdf5
{
class PointCloudIO : public GeneralIO
{
public:
  PointCloudIO(std::shared_ptr<HighFive::File>& file, std::shared_ptr<std::mutex>& write_mtx);

  std::map<std::string, HighFive::Group> getPointClouds();

  std::shared_ptr<HighFive::Group> writePointCloud2(const std::string& uuid, const seerep::PointCloud2& pointcloud2);

  std::optional<seerep::PointCloud2> readPointCloud2(const std::string& uuid);

  void writePointFieldAttributes(HighFive::Group& cloud_group,
                                 const google::protobuf::RepeatedPtrField<seerep::PointField> repeatedPointField);

  google::protobuf::RepeatedPtrField<seerep::PointField> readPointFieldAttributes(HighFive::Group& cloud_group);

private:
  struct CloudInfo
  {
    bool has_points = false;
    bool has_rgb = false;
    bool has_rgba = false;
    bool has_normals = false;
    std::map<std::string, seerep::PointField> other_fields;
  };

  CloudInfo getCloudInfo(const seerep::PointCloud2& cloud);

  void writePoints(const std::string& uuid, const seerep::PointCloud2& cloud);

  void writeColorsRGB(const std::string& uuid, const seerep::PointCloud2& cloud);

  void writeColorsRGBA(const std::string& uuid, const seerep::PointCloud2& cloud);

  void writeOtherFields(const std::string& uuid, const seerep::PointCloud2& cloud,
                        const std::map<std::string, seerep::PointField>& fields);

  void readPoints(const std::string& uuid, seerep::PointCloud2& cloud);

  void readColorsRGB(const std::string& uuid, seerep::PointCloud2& cloud);

  void readColorsRGBA(const std::string& uuid, seerep::PointCloud2& cloud);

  // image / pointcloud attribute keys
  inline static const std::string HEIGHT = "height";
  inline static const std::string WIDTH = "width";
  inline static const std::string ENCODING = "encoding";
  inline static const std::string IS_BIGENDIAN = "is_bigendian";
  inline static const std::string ROW_STEP = "row_step";
  inline static const std::string POINT_STEP = "point_step";
  inline static const std::string IS_DENSE = "is_dense";

  // pointcloud fields attribute keys
  inline static const std::string FIELD_NAME = "field_name_";
  inline static const std::string FIELD_OFFSET = "field_offset_";
  inline static const std::string FIELD_DATATYPE = "field_datatype_";
  inline static const std::string FIELD_COUNT = "field_count_";

  // point and quaternion attribute keys
  const std::string X = "x";
  const std::string Y = "y";
  const std::string Z = "z";
  const std::string W = "w";

public:
  // make private again after fixing io calls of pointcloud.cpp and pointcloud-overview.cpp
  inline static const std::string BOUNDINGBOX = "TODO";  // "boundingbox";
  // datatype group names in hdf5
  inline static const std::string HDF5_GROUP_POINTCLOUD = "pointclouds";
};
} /* namespace seerep_hdf5 */

#endif /* SEEREP_HDF5_IO_POINT_CLOUD_H_ */
