#include "seerep-server/pb-point-cloud-service.h"

namespace seerep_server
{
PbPointCloudService::PbPointCloudService(std::shared_ptr<seerep_core::Core> seerepCore)
  : pointCloudPb(std::make_shared<seerep_core_pb::CorePbPointCloud>(seerepCore))
{
}

grpc::Status PbPointCloudService::GetPointCloud2(grpc::ServerContext* context, const seerep::Query* request,
                                                 grpc::ServerWriter<seerep::PointCloud2>* writer)
{
  (void)context;  // ignore that variable without causing warnings
  std::cout << "sending point cloud in bounding box min(" << request->boundingbox().point_min().x() << "/"
            << request->boundingbox().point_min().y() << "/" << request->boundingbox().point_min().z() << "), max("
            << request->boundingbox().point_max().x() << "/" << request->boundingbox().point_max().y() << "/"
            << request->boundingbox().point_max().z() << ")"
            << " and time interval (" << request->timeinterval().time_min().seconds() << "/"
            << request->timeinterval().time_max().seconds() << ")" << std::endl;

  std::vector<seerep::PointCloud2> pointClouds;
  try
  {
    pointClouds = pointCloudPb->getData(*request);
  }
  catch (std::runtime_error const& e)
  {
    // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
    // also catching core doesn't have project with uuid error
    std::cout << e.what() << std::endl;
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
  }

  if (!pointClouds.empty())
  {
    std::cout << "Found " << pointClouds.size() << " pointclouds that match the query" << std::endl;
    for (const seerep::PointCloud2& pc : pointClouds)
    {
      writer->Write(pc);
    }
  }
  else
  {
    std::cout << "Found NOTHING that matches the query" << std::endl;
  }
  return grpc::Status::OK;
}

grpc::Status PbPointCloudService::TransferPointCloud2(grpc::ServerContext* context,
                                                      const seerep::PointCloud2* pointCloud2,
                                                      seerep::ServerResponse* response)
{
  (void)context;  // ignore that variable without causing warnings
  std::cout << "received point clouds... " << std::endl;

  if (!pointCloud2->header().uuid_project().empty())
  {
    try
    {
      boost::uuids::uuid pointCloudUuid = pointCloudPb->addData(*pointCloud2);

      seerep_server_util::createResponsePb(boost::lexical_cast<std::string>(pointCloudUuid),
                                           seerep::ServerResponse::SUCCESS, response);

      return grpc::Status::OK;
    }
    catch (std::runtime_error const& e)
    {
      // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
      // also catching core doesn't have project with uuid error
      std::cout << e.what() << std::endl;

      seerep_server_util::createResponsePb(std::string(e.what()), seerep::ServerResponse::FAILURE, response);

      return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
    }
  }
  else
  {
    std::cout << "project_uuid is empty!" << std::endl;
    seerep_server_util::createResponsePb("project_uuid is empty!", seerep::ServerResponse::FAILURE, response);

    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "project_uuid is empty!");
  }
}

} /* namespace seerep_server */
