#include "seerep-server/tf-service.h"

namespace seerep_server
{
TfService::TfService(std::shared_ptr<seerep_core::ProjectOverview> projectOverview) : projectOverview(projectOverview)
{
}

grpc::Status TfService::TransferTransformStamped(grpc::ServerContext* context,
                                                 const seerep::TransformStamped* transform,
                                                 seerep::ServerResponse* response)
{
  std::cout << "received transform... " << std::endl;

  if (!transform->header().uuid_project().empty())
  {
    boost::uuids::uuid uuid;
    try
    {
      boost::uuids::string_generator gen;
      uuid = gen(transform->header().uuid_project());
    }
    catch (std::runtime_error e)
    {
      // mainly catching "invalid uuid string"
      std::cout << e.what() << std::endl;
      return grpc::Status::CANCELLED;
    }
    projectOverview->addTF(*transform, uuid);
    response->set_message("added transform");
    response->set_transmission_state(seerep::ServerResponse::SUCCESS);
    return grpc::Status::OK;
  }
  else
  {
    std::cout << "project_uuid is empty!" << std::endl;
    return grpc::Status::CANCELLED;
  }
}

grpc::Status TfService::GetFrames(grpc::ServerContext* context, const seerep::FrameQuery* frameQuery,
                                  seerep::FrameInfos* response)
{
  boost::uuids::uuid uuid;
  try
  {
    boost::uuids::string_generator gen;
    uuid = gen(frameQuery->projectuuid());
  }
  catch (std::runtime_error e)
  {
    // mainly catching "invalid uuid string"
    std::cout << e.what() << std::endl;
    return grpc::Status::CANCELLED;
  }
  for (auto framename : projectOverview->getFrames(uuid))
  {
    response->add_frames(framename);
  }
  return grpc::Status::OK;
}

grpc::Status TfService::GetTransformStamped(grpc::ServerContext* context,
                                            const seerep::TransformStampedQuery* transformQuery,
                                            seerep::TransformStamped* response)
{
  boost::uuids::uuid uuid;
  try
  {
    boost::uuids::string_generator gen;
    uuid = gen(transformQuery->header().uuid_project());
  }
  catch (std::runtime_error e)
  {
    // mainly catching "invalid uuid string"
    std::cout << e.what() << std::endl;
    return grpc::Status::CANCELLED;
  }
  projectOverview->getTF(*transformQuery, uuid);

  return grpc::Status::OK;
}
} /* namespace seerep_server */