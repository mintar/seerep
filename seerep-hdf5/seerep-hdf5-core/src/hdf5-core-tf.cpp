#include "seerep-hdf5-core/hdf5-core-tf.h"

#include <highfive/H5DataSet.hpp>

namespace seerep_hdf5_core
{
Hdf5CoreTf::Hdf5CoreTf(std::shared_ptr<HighFive::File>& file, std::shared_ptr<std::mutex>& write_mtx)
  : Hdf5CoreGeneral(file, write_mtx)
{
}

// split this method
std::optional<std::vector<geometry_msgs::TransformStamped>> Hdf5CoreTf::readTransformStamped(const std::string& id)
{
  const std::scoped_lock lock(*m_write_mtx);

  std::string hdf5GroupPath = HDF5_GROUP_TF + "/" + id;
  std::string hdf5DatasetTimePath = hdf5GroupPath + "/" + "time";
  std::string hdf5DatasetTransPath = hdf5GroupPath + "/" + "translation";
  std::string hdf5DatasetRotPath = hdf5GroupPath + "/" + "rotation";

  if (!m_file->exist(hdf5GroupPath) || !m_file->exist(hdf5DatasetTimePath) || !m_file->exist(hdf5DatasetTransPath) ||
      !m_file->exist(hdf5DatasetRotPath))
  {
    return std::nullopt;
  }

  BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "loading " << hdf5GroupPath;

  // read size
  std::shared_ptr<HighFive::Group> group_ptr = std::make_shared<HighFive::Group>(m_file->getGroup(hdf5GroupPath));
  long unsigned int size;
  group_ptr->getAttribute(SIZE).read(size);
  if (size == 0)
  {
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "tf data has size 0.";
    return std::nullopt;
  }

  // read frames
  std::string parentframe;
  group_ptr->getAttribute("PARENT_FRAME").read(parentframe);
  std::string childframe;
  group_ptr->getAttribute("CHILD_FRAME").read(childframe);

  // read time
  std::shared_ptr<HighFive::DataSet> data_set_time_ptr =
      std::make_shared<HighFive::DataSet>(m_file->getDataSet(hdf5DatasetTimePath));
  std::vector<std::vector<int64_t>> time;
  data_set_time_ptr->read(time);

  // read translation
  std::shared_ptr<HighFive::DataSet> data_set_trans_ptr =
      std::make_shared<HighFive::DataSet>(m_file->getDataSet(hdf5DatasetTransPath));
  std::vector<std::vector<double>> trans;
  data_set_trans_ptr->read(trans);

  // read rotation
  std::shared_ptr<HighFive::DataSet> data_set_rot_ptr =
      std::make_shared<HighFive::DataSet>(m_file->getDataSet(hdf5DatasetRotPath));
  std::vector<std::vector<double>> rot;
  data_set_rot_ptr->read(rot);

  // check if all have the right size
  if (time.size() != size || trans.size() != size || rot.size() != size)
  {
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::warning)
        << "sizes of time (" << time.size() << "), translation (" << trans.size() << ") and rotation (" << rot.size()
        << ") not matching. Size expected by value in metadata (" << size << ")";
    return std::nullopt;
  }

  std::vector<geometry_msgs::TransformStamped> tfs;
  for (long unsigned int i = 0; i < size; i++)
  {
    geometry_msgs::TransformStamped tf;
    tf.header.frame_id = parentframe;
    tf.child_frame_id = childframe;

    tf.header.stamp.sec = time.at(i).at(0);
    tf.header.stamp.nsec = time.at(i).at(1);

    tf.transform.translation.x = trans.at(i).at(0);
    tf.transform.translation.y = trans.at(i).at(1);
    tf.transform.translation.z = trans.at(i).at(2);

    tf.transform.rotation.x = rot.at(i).at(0);
    tf.transform.rotation.y = rot.at(i).at(1);
    tf.transform.rotation.z = rot.at(i).at(2);
    tf.transform.rotation.w = rot.at(i).at(3);

    tfs.push_back(tf);
  }
  return tfs;
}

std::optional<std::vector<std::string>> Hdf5CoreTf::readTransformStampedFrames(const std::string& id)
{
  const std::scoped_lock lock(*m_write_mtx);

  std::string hdf5GroupPath = HDF5_GROUP_TF + "/" + id;

  if (!m_file->exist(hdf5GroupPath))
  {
    return std::nullopt;
  }

  std::shared_ptr<HighFive::Group> group_ptr = std::make_shared<HighFive::Group>(m_file->getGroup(hdf5GroupPath));

  BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "loading parent frame of " << hdf5GroupPath;

  // read frames
  std::string parentframe;
  group_ptr->getAttribute("PARENT_FRAME").read(parentframe);
  std::string childframe;
  group_ptr->getAttribute("CHILD_FRAME").read(childframe);

  return std::vector<std::string>{ parentframe, childframe };
}

}  // namespace seerep_hdf5_core
