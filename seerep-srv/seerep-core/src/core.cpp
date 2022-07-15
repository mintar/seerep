#include "seerep-core/core.h"

namespace seerep_core
{
Core::Core(std::string dataFolder, bool loadHdf5Files) : m_dataFolder(dataFolder)
{
  if (loadHdf5Files)
  {
    recreateProjects();
  }
}

Core::~Core()
{
}

seerep_core_msgs::QueryResult Core::getDataset(const seerep_core_msgs::Query& query)
{
  seerep_core_msgs::QueryResult result;

  // divide into at least two methods and see if you can reduce the duplicated code

  // search all projects
  if (!query.projects)
  {
    for (auto& it : m_projects)
    {
      auto dataset = it.second->getDataset(query);
      if (!dataset.dataOrInstanceUuids.empty())  // same call
      {
        result.queryResultProjects.push_back(dataset);
      }
    }
  }
  // Search only in project specified in query
  else
  {
    for (auto projectuuid : query.projects.value())
    {
      auto project = m_projects.find(projectuuid);
      if (project != m_projects.end())
      {
        auto dataset = project->second->getDataset(query);
        if (!dataset.dataOrInstanceUuids.empty())  // same call
        {
          result.queryResultProjects.push_back(dataset);
        }
      }
      // if not found throw error
      else
      {
        throw std::runtime_error("project " + boost::lexical_cast<std::string>(query.header.uuidProject) +
                                 "does not exist!");
      };
    }
  }

  return result;
}

seerep_core_msgs::QueryResult Core::getInstances(const seerep_core_msgs::Query& query)
{
  seerep_core_msgs::QueryResult result;

  // duplicated code

  // search all projects
  if (!query.projects)
  {
    for (auto& it : m_projects)
    {
      auto instances = it.second->getInstances(query);
      if (!instances.dataOrInstanceUuids.empty())
      {
        result.queryResultProjects.push_back(instances);
      }
    }
  }
  // Search only in project specified in query
  else
  {
    for (auto projectuuid : query.projects.value())
    {
      auto project = m_projects.find(projectuuid);
      if (project != m_projects.end())
      {
        // change to getInstance
        auto instances = project->second->getDataset(query);
        if (!instances.dataOrInstanceUuids.empty())
        {
          result.queryResultProjects.push_back(instances);
        }
      }
      // if not found throw error
      else
      {
        throw std::runtime_error("project " + boost::lexical_cast<std::string>(query.header.uuidProject) +
                                 "does not exist!");
      };
    }
  }

  return result;
}

void Core::recreateProjects()
{
  for (const auto& entry : std::filesystem::directory_iterator(m_dataFolder))
  {
    if (entry.path().filename().extension() == ".h5")
    {
      BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info)
          << "found " << entry.path().string() << " in HDF5 file.";

      try
      {
        boost::uuids::string_generator gen;
        boost::uuids::uuid uuid = gen(entry.path().filename().stem().string());

        auto project = std::make_shared<CoreProject>(uuid, entry.path().string());
        m_projects.insert(std::make_pair(uuid, project));
      }
      catch (const std::runtime_error& e)
      {
        BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::error) << e.what();
      }
    }
  }
}

// method name: createProject
void Core::newProject(const seerep_core_msgs::ProjectInfo& projectInfo)
{
  std::string filename = boost::lexical_cast<std::string>(projectInfo.uuid);
  std::string path = m_dataFolder + "/" + filename + ".h5";

  auto project = std::make_shared<CoreProject>(projectInfo.uuid, path, projectInfo.name, projectInfo.frameId);
  m_projects.insert(std::make_pair(projectInfo.uuid, project));
}

std::vector<seerep_core_msgs::ProjectInfo> Core::getProjects()
{
  std::vector<seerep_core_msgs::ProjectInfo> projectInfos;
  for (auto it = m_projects.begin(); it != m_projects.end(); ++it)
  {
    seerep_core_msgs::ProjectInfo projectinfo;
    projectinfo.name = it->second->getName();
    projectinfo.uuid = it->first;
    projectinfo.frameId = it->second->getFrameId();

    projectInfos.push_back(projectinfo);
  }

  return projectInfos;
}

// create a private method, that will return the project and use that, instead of doing the same thing over and over again

/**
 * as an example for addDataset; the rest could look similar
 *
 * void Core::addDataset(const seerep_core_msgs::DatasetIndexable& dataset)
 * {
 *  try {
 *  project = findProject(dataset.header.uuidProject)
 *
 *  project->second->addDataset(dataset)
 * } catch {
 *  handle catch-stuff
 * }
 *
 * }
 *
 * void Core:findProejct(const uuidProject)
 * {
 *  auto project = m_project.find(uuidProject)
 * if (project != m_project.end()) {
 *  return project
 * } else {
 *  do something here, like throw an exception or so
 * }
 * }
 * */

void Core::addDataset(const seerep_core_msgs::DatasetIndexable& dataset)
{
  // find the project based on its uuid
  auto project = m_projects.find(dataset.header.uuidProject);
  // if project was found add image
  if (project != m_projects.end())
  {
    return project->second->addDataset(dataset);
  }
  // if not found throw error
  else
  {
    throw std::runtime_error("project " + boost::lexical_cast<std::string>(dataset.header.uuidProject) +
                             "does not exist!");
  };
}

void Core::addLabels(const seerep_core_msgs::Datatype& datatype, std::vector<std::string>& labels,
                     const boost::uuids::uuid& msgUuid, const boost::uuids::uuid& projectuuid)
{
  // find the project based on its uuid
  auto project = m_projects.find(projectuuid);
  // if project was found add image
  if (project != m_projects.end())
  {
    return project->second->addLabels(datatype, labels, msgUuid);
  }
  // if not found throw error
  else
  {
    throw std::runtime_error("project " + boost::lexical_cast<std::string>(projectuuid) + "does not exist!");
  };
}

void Core::addTF(const geometry_msgs::TransformStamped& tf, const boost::uuids::uuid& projectuuid)
{
  // find the project based on its uuid
  auto project = m_projects.find(projectuuid);
  // if project was found add tf
  if (project != m_projects.end())
  {
    return project->second->addTF(tf);
  }
  // if not found throw error
  else
  {
    throw std::runtime_error("project " + boost::lexical_cast<std::string>(projectuuid) + "does not exist!");
  };
}

std::optional<geometry_msgs::TransformStamped> Core::getTF(const seerep_core_msgs::QueryTf& transformQuery)
{
  // find the project based on its uuid
  auto project = m_projects.find(transformQuery.project);
  // if project was found call function and return result
  if (project != m_projects.end())
  {
    return project->second->getTF(transformQuery);
  }
  // if not found return empty optional
  else
  {
    return std::nullopt;
  };
}

std::vector<std::string> Core::getFrames(const boost::uuids::uuid& projectuuid)
{
  // find the project based on its uuid
  auto project = m_projects.find(projectuuid);
  // if project was found call function and return result
  if (project != m_projects.end())
  {
    return project->second->getFrames();
  }
  // if not found return empty vector
  else
  {
    return {};
  };
}

std::shared_ptr<std::mutex> Core::getHdf5FileMutex(const boost::uuids::uuid& projectuuid)
{
  // find the project based on its uuid
  auto project = m_projects.find(projectuuid);
  // if project was found return pointer to mutex
  if (project != m_projects.end())
  {
    return project->second->getHdf5FileMutex();
  }
  // if not found return null pointer
  else
  {
    // does that result in errors on the callee side?
    return nullptr;
  }
}
std::shared_ptr<HighFive::File> Core::getHdf5File(const boost::uuids::uuid& projectuuid)
{
  // find the project based on its uuid
  auto project = m_projects.find(projectuuid);
  // if project was found return pointer to HighFive::File
  if (project != m_projects.end())
  {
    return project->second->getHdf5File();
  }
  // if not found return null pointer
  else
  {
    return nullptr;
  }
}

} /* namespace seerep_core */
