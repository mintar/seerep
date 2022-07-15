#include "seerep-core/core-instance.h"

namespace seerep_core
{
CoreInstance::CoreInstance(std::shared_ptr<seerep_hdf5_core::Hdf5CoreInstance> hdf5_io,
                           const boost::uuids::uuid& uuidInstance)
  : m_hdf5_io(hdf5_io), m_uuid(uuidInstance)
{
  recreateInstance();

  // fill the map with empty vectors for each implemented datatype
  m_typeUUIDMap.emplace(seerep_core_msgs::Datatype::Images, std::vector<boost::uuids::uuid>());
  m_typeUUIDMap.emplace(seerep_core_msgs::Datatype::PointClouds, std::vector<boost::uuids::uuid>());
}
CoreInstance::~CoreInstance()
{
}

boost::uuids::uuid CoreInstance::getUUID()
{
  return m_uuid;
}

std::optional<std::string> CoreInstance::getAttribute(const std::string& key) const
{
  // value is not such a good name for a variable
  auto value = m_attributes.find(key);

  if (value != m_attributes.end())
  {
    return value->second;
  }
  // unnecessary else-branch
  else
  {
    return std::nullopt;
  }
}

void CoreInstance::writeAttribute(const std::string& key, const std::string& value)
{
  auto emplaceResult = m_attributes.emplace(key, value);

  // key already in map -> override!
  // what if not?
  if (!emplaceResult.second)
  {
    m_attributes.at(key) = value;
  }

  m_hdf5_io->writeAttribute(m_uuid, key, value);
}

std::vector<boost::uuids::uuid> CoreInstance::getDatasets(const seerep_core_msgs::Datatype& datatype) const
{
  return m_typeUUIDMap.at(datatype);
}

void CoreInstance::addDataset(const boost::uuids::uuid& uuidDataset, const seerep_core_msgs::Datatype& datatype)
{
  m_typeUUIDMap.at(datatype).push_back(uuidDataset);
}

void CoreInstance::recreateInstance()
{
  auto attributes = m_hdf5_io->readAttributes(m_uuid);

  if (attributes)
  {
    m_attributes = std::move(*attributes);
  }
}
} /* namespace seerep_core */
