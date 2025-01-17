#ifndef SEEREP_HDF5_HDF5_GENERAL_CORE_H_
#define SEEREP_HDF5_HDF5_GENERAL_CORE_H_

// highfive
#include <highfive/H5File.hpp>

// seerep-msgs
#include <seerep-msgs/aabb.h>
#include <seerep-msgs/dataset-indexable.h>

// std
#include <boost/geometry.hpp>
#include <mutex>
#include <optional>

// logging
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

namespace seerep_hdf5_core
{
class Hdf5CoreGeneral
{
public:
  Hdf5CoreGeneral(std::shared_ptr<HighFive::File>& file, std::shared_ptr<std::mutex>& write_mtx);

  std::vector<std::string> getGroupDatasets(const std::string& id);
  void checkExists(const std::string& id);
  std::optional<std::string> readFrameId(const std::string& datatypeGroup, const std::string& uuid);
  //################
  // Attributes
  //################
  template <typename T, class C>
  T readAttributeFromHdf5(const std::string& id, const HighFive::AnnotateTraits<C>& object, std::string attributeField);

  template <typename T, class C>
  void writeAttributeToHdf5(HighFive::AnnotateTraits<C>& object, std::string attributeField, T attributeValue);

  void deleteAttribute(const std::shared_ptr<HighFive::DataSet> dataSetPtr, std::string attributeField);
  //################
  // AABB
  //################
  void writeAABB(
      const std::string& datatypeGroup, const std::string& uuid,
      const boost::geometry::model::box<boost::geometry::model::point<float, 3, boost::geometry::cs::cartesian>>& aabb);

  void
  readAABB(const std::string& datatypeGroup, const std::string& uuid,
           boost::geometry::model::box<boost::geometry::model::point<float, 3, boost::geometry::cs::cartesian>>& aabb);

  bool hasAABB(const std::string& datatypeGroup, const std::string& uuid);

  //################
  // Time
  //################
  void readTimeFromRaw(const std::string& datatypeGroup, const std::string& uuid, int64_t& secs, int64_t& nanos);
  void readTime(const std::string& datatypeGroup, const std::string& uuid, int64_t& secs, int64_t& nanos);
  template <class T>
  void readTimeFromAnnotateTraits(const std::string& id, int64_t& value,
                                  const HighFive::AnnotateTraits<T>& highFiveObject, const std::string& attribute);

  void writeTimeToRaw(const std::string& datatypeGroup, const std::string& uuid, const int64_t& secs,
                      const int64_t& nanos);
  void writeTime(const std::string& datatypeGroup, const std::string& uuid, const int64_t& secs, const int64_t& nanos);
  template <class T>
  void writeTimeToAnnotateTraits(const int64_t& value, HighFive::AnnotateTraits<T>& highFiveObject,
                                 const std::string& attribute);

  bool hasTimeRaw(const std::string& datatypeGroup, const std::string& uuid);
  bool hasTime(const std::string& datatypeGroup, const std::string& uuid);

  // BoundingBoxes
  void readBoundingBoxLabeled(const std::string& datatypeGroup, const std::string& uuid,
                              std::vector<std::string>& labels, std::vector<std::vector<double>>& boundingBoxes,
                              std::vector<std::string>& instances, bool loadBoxes = true);

  // Labels General
  void readLabelsGeneral(const std::string& datatypeGroup, const std::string& uuid, std::vector<std::string>& labels,
                         std::vector<std::string>& instances);

  // //################
  // // Project
  // //################
  void writeProjectname(const std::string& projectname);
  std::string readProjectname();

  void writeProjectFrameId(const std::string& frameId);
  std::string readProjectFrameId();

private:
  void readLabel(const std::string& id, const std::string labelType, std::vector<std::string>& labels);
  void readBoundingBoxes(const std::string& id, const std::string boundingBoxType,
                         std::vector<std::vector<double>>& boundingBoxes);
  void readInstances(const std::string& id, const std::string InstanceType, std::vector<std::string>& instances);

public:
  // header attribute keys
  inline static const std::string HEADER_STAMP_SECONDS = "header_stamp_seconds";
  inline static const std::string HEADER_STAMP_NANOS = "header_stamp_nanos";
  inline static const std::string HEADER_FRAME_ID = "header_frame_id";
  inline static const std::string HEADER_SEQ = "header_seq";

  inline static const std::string AABB_FIELD = "AABB";

  inline static const std::string PROJECTNAME = "projectname";
  inline static const std::string PROJECTFRAMEID = "projectframeid";

  // dataset names
  inline static const std::string RAWDATA = "rawdata";
  inline static const std::string LABELGENERAL = "labelGeneral";
  inline static const std::string LABELGENERALINSTANCES = "labelGeneralInstances";
  inline static const std::string LABELBB = "labelBB";
  inline static const std::string LABELBBBOXES = "labelBBBoxes";
  inline static const std::string LABELBBINSTANCES = "labelBBInstances";

protected:
  std::shared_ptr<HighFive::File> m_file;
  std::shared_ptr<std::mutex> m_write_mtx;
  boost::log::sources::severity_logger<boost::log::trivial::severity_level> m_logger;
};

}  // namespace seerep_hdf5_core

#include "impl/hdf5-core-general.hpp"  // NOLINT

#endif /* SEEREP_HDF5_HDF5_GENERAL_CORE_H_ */
