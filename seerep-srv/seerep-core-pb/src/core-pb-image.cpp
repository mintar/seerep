#include "seerep-core-pb/core-pb-image.h"

namespace seerep_core_pb
{
CorePbImage::CorePbImage(std::shared_ptr<seerep_core::Core> seerepCore) : m_seerepCore(seerepCore)
{
  for (seerep_core_msgs::ProjectInfo projectInfo : m_seerepCore->getProjects())
  {
    getFileAccessorFromCore(projectInfo.uuid);
  }
}

CorePbImage::~CorePbImage()
{
}

std::vector<seerep::Image> CorePbImage::getData(const seerep::Query& query)
{
  std::cout << "loading image from images/" << std::endl;
  seerep_core_msgs::Query queryCore;
  queryCore.header.datatype = seerep_core_msgs::Datatype::Images;
  boost::uuids::string_generator gen;
  queryCore.projects.value().push_back(gen(query.projectuuid()));

  // duplicate from from core-pb-pointcloud.cpp

  for (auto label : query.label())
  {
    queryCore.label.value().push_back(label);
  }
  queryCore.timeinterval.value().timeMin.seconds = query.timeinterval().time_min().seconds();
  queryCore.timeinterval.value().timeMax.seconds = query.timeinterval().time_max().seconds();
  queryCore.timeinterval.value().timeMin.nanos = query.timeinterval().time_min().nanos();
  queryCore.timeinterval.value().timeMax.nanos = query.timeinterval().time_max().nanos();

  queryCore.header.frameId = query.boundingbox().header().frame_id();
  queryCore.boundingbox.value().min_corner().set<0>(query.boundingbox().point_min().x());
  queryCore.boundingbox.value().min_corner().set<1>(query.boundingbox().point_min().y());
  queryCore.boundingbox.value().min_corner().set<2>(query.boundingbox().point_min().z());
  queryCore.boundingbox.value().max_corner().set<0>(query.boundingbox().point_max().x());
  queryCore.boundingbox.value().max_corner().set<1>(query.boundingbox().point_max().y());
  queryCore.boundingbox.value().max_corner().set<2>(query.boundingbox().point_max().z());

  seerep_core_msgs::QueryResult resultCore = m_seerepCore->getDataset(queryCore);

  std::vector<seerep::Image> resultImages;
  for (auto project : resultCore.queryResultProjects)
  {
    for (auto uuidImg : project.dataOrInstanceUuids)
    {
      auto hdf5io = getHdf5(project.projectUuid);
      std::optional<seerep::Image> image = hdf5io->readImage(boost::lexical_cast<std::string>(uuidImg));
      if (image)
      {
        resultImages.push_back(image.value());
      }
    }
  }
  return resultImages;
}

boost::uuids::uuid CorePbImage::addData(const seerep::Image& img)
{
  boost::uuids::string_generator gen;
  boost::uuids::uuid uuid;
  if (img.header().uuid_msgs().empty())
  {
    uuid = boost::uuids::random_generator()();
  }
  else
  {
    uuid = gen(img.header().uuid_msgs());
  }
  auto hdf5io = getHdf5(gen(img.header().uuid_project()));
  hdf5io->writeImage(boost::lexical_cast<std::string>(uuid), img);

  seerep_core_msgs::DatasetIndexable dataForIndices;
  dataForIndices.header.datatype = seerep_core_msgs::Datatype::Images;
  dataForIndices.header.frameId = img.header().frame_id();
  dataForIndices.header.timestamp.seconds = img.header().stamp().seconds();
  dataForIndices.header.timestamp.nanos = img.header().stamp().nanos();
  dataForIndices.header.uuidData = uuid;
  dataForIndices.header.uuidProject = gen(img.header().uuid_project());
  // set bounding box for images to 0. assume no spatial extent
  dataForIndices.boundingbox.min_corner().set<0>(0);
  dataForIndices.boundingbox.min_corner().set<1>(0);
  dataForIndices.boundingbox.min_corner().set<2>(0);
  dataForIndices.boundingbox.max_corner().set<0>(0);
  dataForIndices.boundingbox.max_corner().set<1>(0);
  dataForIndices.boundingbox.max_corner().set<2>(0);

  // semantic
  dataForIndices.labelsWithInstances.reserve(img.labels_general().size() + img.labels_bb().size());
  for (auto label : img.labels_general())
  {
    boost::uuids::string_generator gen;
    boost::uuids::uuid uuidInstance;
    try
    {
      uuidInstance = gen(label.instanceuuid());
    }
    catch (std::runtime_error const& e)
    {
      uuidInstance = boost::uuids::nil_uuid();
    }

    dataForIndices.labelsWithInstances.push_back(
        seerep_core_msgs::LabelWithInstance{ .label = label.label(), .uuidInstance = uuidInstance });
  }

  for (auto label : img.labels_bb())
  {
    boost::uuids::string_generator gen;
    boost::uuids::uuid uuidInstance;
    try
    {
      uuidInstance = gen(label.labelwithinstance().instanceuuid());
    }
    catch (std::runtime_error const& e)
    {
      uuidInstance = boost::uuids::nil_uuid();
    }
    dataForIndices.labelsWithInstances.push_back(seerep_core_msgs::LabelWithInstance{
        .label = label.labelwithinstance().label(), .uuidInstance = uuidInstance });
  }

  m_seerepCore->addDataset(dataForIndices);

  return uuid;
}

void CorePbImage::getFileAccessorFromCore(boost::uuids::uuid project)
{
  auto hdf5file = m_seerepCore->getHdf5File(project);
  auto hdf5fileMutex = m_seerepCore->getHdf5FileMutex(project);
  auto imageIo = std::make_shared<seerep_hdf5_pb::Hdf5PbImage>(hdf5file, hdf5fileMutex);
  m_hdf5IoMap.insert(std::make_pair(project, imageIo));
}

std::shared_ptr<seerep_hdf5_pb::Hdf5PbImage> CorePbImage::getHdf5(boost::uuids::uuid project)
{
  // find the project based on its uuid
  auto hdf5io = m_hdf5IoMap.find(project);
  // if project was found add tf
  if (hdf5io != m_hdf5IoMap.end())
  {
    return hdf5io->second;
  }
  // if not found ask core
  else
  {
    // this throws an exeption if core has no project with the uuid
    getFileAccessorFromCore(project);
    // if getFileAccessorFromCore didn't throw an error, find project and return pointer
    return m_hdf5IoMap.find(project)->second;
  };
}

}  // namespace seerep_core_pb
