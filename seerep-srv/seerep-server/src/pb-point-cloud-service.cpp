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
  BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info)
      << "sending point cloud in bounding box min(" << request->boundingbox().point_min().x() << "/"
      << request->boundingbox().point_min().y() << "/" << request->boundingbox().point_min().z() << "), max("
      << request->boundingbox().point_max().x() << "/" << request->boundingbox().point_max().y() << "/"
      << request->boundingbox().point_max().z() << ")"
      << " and time interval (" << request->timeinterval().time_min() << "/" << request->timeinterval().time_max()
      << ")";

  std::vector<seerep::PointCloud2> pointClouds;
  try
  {
    pointClouds = pointCloudPb->getData(*request);
  }
  catch (std::runtime_error const& e)
  {
    // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
    // also catching core doesn't have project with uuid error
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::error) << e.what();
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
  }

  if (!pointClouds.empty())
  {
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info)
        << "Found " << pointClouds.size() << " pointclouds that match the query";
    for (const seerep::PointCloud2& pc : pointClouds)
    {
      writer->Write(pc);
    }
  }
  else
  {
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "Found NOTHING that matches the query";
  }
  return grpc::Status::OK;
}

grpc::Status PbPointCloudService::TransferPointCloud2(grpc::ServerContext* context,
                                                      const seerep::PointCloud2* pointCloud2,
                                                      seerep::ServerResponse* response)
{
  (void)context;  // ignore that variable without causing warnings
  BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "received point clouds... ";

  if (!pointCloud2->header().uuid_project().empty())
  {
    boost::uuids::uuid uuid;
    try
    {
      boost::uuids::string_generator gen;
      uuid = gen(pointCloud2->header().uuid_project());

      boost::uuids::uuid uuidImg = pointCloudPb->addData(*pointCloud2);

      response->set_message(boost::lexical_cast<std::string>(uuidImg));
      response->set_transmission_state(seerep::ServerResponse::SUCCESS);
      return grpc::Status::OK;
    }
    catch (std::runtime_error const& e)
    {
      // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
      // also catching core doesn't have project with uuid error
      BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::error) << e.what();
      response->set_message(std::string(e.what()));
      response->set_transmission_state(seerep::ServerResponse::FAILURE);
      return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
    }
  }
  else
  {
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "project_uuid is empty!";
    response->set_message("project_uuid is empty!");
    response->set_transmission_state(seerep::ServerResponse::FAILURE);
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "project_uuid is empty!");
  }
}

} /* namespace seerep_server */
