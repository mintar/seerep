#include "seerep-server/pb-meta-operations.h"

namespace seerep_server
{
PbMetaOperations::PbMetaOperations(std::shared_ptr<seerep_core::Core> seerepCore) : seerepCore(seerepCore)
{
}

grpc::Status PbMetaOperations::CreateProject(grpc::ServerContext* context, const seerep::ProjectCreation* request,
                                             seerep::ProjectInfo* response)
{
  (void)context;  // ignore that variable without causing warnings
  BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "create new project... ";
  seerep_core_msgs::ProjectInfo projectInfo;
  projectInfo.frameId = request->mapframeid();
  projectInfo.name = request->name();
  projectInfo.uuid = boost::uuids::random_generator()();
  seerepCore->newProject(projectInfo);

  response->set_name(projectInfo.name);
  response->set_uuid(boost::lexical_cast<std::string>(projectInfo.uuid));

  return grpc::Status::OK;
}

grpc::Status PbMetaOperations::GetProjects(grpc::ServerContext* context, const google::protobuf::Empty* request,
                                           seerep::ProjectInfos* response)
{
  (void)context;  // ignore that variable without causing warnings
  (void)request;  // ignore that variable without causing warnings
  BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "query the project infos... ";
  auto projectInfos = seerepCore->getProjects();

  for (auto projectInfo : projectInfos)
  {
    auto responseProjectInfo = response->add_projects();
    responseProjectInfo->set_name(projectInfo.name);
    responseProjectInfo->set_uuid(boost::lexical_cast<std::string>(projectInfo.uuid));
  }

  return grpc::Status::OK;
}

} /* namespace seerep_server */
