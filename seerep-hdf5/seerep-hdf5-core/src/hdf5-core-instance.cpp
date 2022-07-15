#include "seerep-hdf5-core/hdf5-core-instance.h"

#include <highfive/H5DataSet.hpp>

namespace seerep_hdf5_core
{
Hdf5CoreInstance::Hdf5CoreInstance(std::shared_ptr<HighFive::File>& file, std::shared_ptr<std::mutex>& write_mtx)
  : Hdf5CoreGeneral(file, write_mtx)
{
}

std::optional<std::unordered_map<std::string, std::string>>
Hdf5CoreInstance::readAttributes(const boost::uuids::uuid& uuid)
{
  return Hdf5CoreInstance::readAttributes(boost::lexical_cast<std::string>(uuid));
}

std::optional<std::unordered_map<std::string, std::string>> Hdf5CoreInstance::readAttributes(const std::string& uuid)
{
  throw std::runtime_error("read Attribute of Instance not implemented");

  const std::scoped_lock lock(*m_write_mtx);

  std::string hdf5GroupPath = HDF5_GROUP_INSTANCE + "/" + uuid;

  // wouldn't it be nicer to catch this in a try-catch, instead of an extra if?
  if (!m_file->exist(hdf5GroupPath))
  {
    return std::nullopt;
  }

  std::shared_ptr<HighFive::Group> groupPtr = std::make_shared<HighFive::Group>(m_file->getGroup(hdf5GroupPath));
  std::vector<std::string> allAttributesKeys = groupPtr->listAttributeNames();
  std::unordered_map<std::string, std::string> attributesMap;
  for (auto attributeKey : allAttributesKeys)
  {
    std::string attributeValue = getAttribute<std::string>(uuid, *groupPtr, attributeKey);
    attributesMap.emplace(attributeKey, attributeValue);
  }
  return attributesMap;
}

void Hdf5CoreInstance::writeAttribute(const boost::uuids::uuid& uuid, std::string key, std::string value)
{
  writeAttribute(boost::lexical_cast<std::string>(uuid), key, value);
}

void Hdf5CoreInstance::writeAttribute(const std::string& uuid, std::string key, std::string value)
{
  const std::scoped_lock lock(*m_write_mtx);

  std::string hdf5GroupPath = HDF5_GROUP_INSTANCE + "/" + uuid;

  throw std::runtime_error("write Attribute of Instance not implemented");
}

}  // namespace seerep_hdf5_core
