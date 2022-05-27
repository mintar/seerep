#ifndef SEEREP_CORE_PB_POINTCLOUD_H_
#define SEEREP_CORE_PB_POINTCLOUD_H_

#include <functional>
#include <optional>

// seerep-msgs
#include <seerep-msgs/point_cloud_2.pb.h>
#include <seerep-msgs/query.pb.h>
// seerep-core-msgs
#include <seerep-msgs/query-result.h>
#include <seerep-msgs/query.h>
// seerep-hdf5-pb
#include <seerep-hdf5-pb/hdf5-pb-pointcloud.h>
// seerep-core
#include <seerep-core/core.h>

// uuid
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>             // uuid class
#include <boost/uuid/uuid_generators.hpp>  // generators
#include <boost/uuid/uuid_io.hpp>          // streaming operators etc.

// logging
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

namespace seerep_core_pb
{
class CorePbPointCloud
{
public:
  CorePbPointCloud(std::shared_ptr<seerep_core::Core> seerepCore);
  ~CorePbPointCloud();

  std::vector<seerep::PointCloud2> getData(const seerep::Query& query);
  boost::uuids::uuid addData(const seerep::PointCloud2& pc);

private:
  void getFileAccessorFromCore(boost::uuids::uuid project);
  std::shared_ptr<seerep_hdf5_pb::Hdf5PbPointCloud> getHdf5(boost::uuids::uuid project);
  std::shared_ptr<seerep_core::Core> m_seerepCore;
  std::unordered_map<boost::uuids::uuid, std::shared_ptr<seerep_hdf5_pb::Hdf5PbPointCloud>,
                     boost::hash<boost::uuids::uuid>>
      m_hdf5IoMap;
  boost::log::sources::severity_logger<boost::log::trivial::severity_level> m_logger;
};

}  // namespace seerep_core_pb

#endif  // SEEREP_CORE_PB_POINTCLOUD_H_
