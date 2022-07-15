#include "seerep-server/fb-image-service.h"

namespace seerep_server
{
FbImageService::FbImageService(std::shared_ptr<seerep_core::Core> seerepCore)
  : imageFb(std::make_shared<seerep_core_fb::CoreFbImage>(seerepCore))
{
}

grpc::Status FbImageService::GetImage(grpc::ServerContext* context,
                                      const flatbuffers::grpc::Message<seerep::fb::Query>* request,
                                      grpc::ServerWriter<flatbuffers::grpc::Message<seerep::fb::Image>>* writer)
{
  (void)context;  // ignore that variable without causing warnings
  auto requestRoot = request->GetRoot();

  std::stringstream debuginfo;
  debuginfo << "sending images with this query parameters:";
  if (requestRoot->boundingbox() != NULL)
  {
    debuginfo << "bounding box min(" << requestRoot->boundingbox()->point_min()->x() << "/"
              << requestRoot->boundingbox()->point_min()->y() << "/" << requestRoot->boundingbox()->point_min()->z()
              << "), max(" << requestRoot->boundingbox()->point_max()->x() << "/"
              << requestRoot->boundingbox()->point_max()->y() << "/" << requestRoot->boundingbox()->point_max()->z()
              << ")";
  }
  if (requestRoot->timeinterval() != NULL)
  {
    debuginfo << "time interval (" << requestRoot->timeinterval()->time_min()->seconds() << "/"
              << requestRoot->timeinterval()->time_max()->seconds() << ")";
  }
  if (requestRoot->label() != NULL)
  {
    debuginfo << "labels general";
    for (auto label : *requestRoot->label())
    {
      debuginfo << "'" << label->str() << "' ";
    }
  }

  BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << debuginfo.rdbuf();

  if (requestRoot->boundingbox() != NULL)
  {
    std::cout << "in bounding box min(" << requestRoot->boundingbox()->point_min()->x() << "/"
              << requestRoot->boundingbox()->point_min()->y() << "/" << requestRoot->boundingbox()->point_min()->z()
              << "), max(" << requestRoot->boundingbox()->point_max()->x() << "/"
              << requestRoot->boundingbox()->point_max()->y() << "/" << requestRoot->boundingbox()->point_max()->z()
              << ")" << std::endl;
  }
  if (requestRoot->timeinterval() != NULL)
  {
    std::cout << "in time interval (" << requestRoot->timeinterval()->time_min()->seconds() << "/"
              << requestRoot->timeinterval()->time_max()->seconds() << ")" << std::endl;
  }
  try
  {
    imageFb->getData(*requestRoot, writer);
  }
  catch (std::runtime_error const& e)
  {
    // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
    // also catching core doesn't have project with uuid error
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::error) << e.what();
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
  }

  return grpc::Status::OK;
}

grpc::Status FbImageService::TransferImage(grpc::ServerContext* context,
                                           grpc::ServerReader<flatbuffers::grpc::Message<seerep::fb::Image>>* reader,
                                           flatbuffers::grpc::Message<seerep::fb::ServerResponse>* response)
{
  (void)context;  // ignore that variable without causing warnings
  std::string answer = "everything stored!";

  flatbuffers::grpc::Message<seerep::fb::Image> imageMsg;
  while (reader->Read(&imageMsg))
  {
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "received image... ";
    auto image = imageMsg.GetRoot();

    std::string uuidProject = image->header()->uuid_project()->str();
    if (!uuidProject.empty())
    {
      try
      {
        imageFb->addData(*image);
      }
      catch (std::runtime_error const& e)
      {
        // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
        // also catching core doesn't have project with uuid error
        BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::error) << e.what();

        // create a method for this
        flatbuffers::grpc::MessageBuilder builder;
        auto msg = builder.CreateString(std::string(e.what()));
        seerep::fb::ServerResponseBuilder responseBuilder(builder);
        responseBuilder.add_message(msg);
        responseBuilder.add_transmission_state(seerep::fb::TRANSMISSION_STATE_FAILURE);
        auto responseOffset = responseBuilder.Finish();
        builder.Finish(responseOffset);
        *response = builder.ReleaseMessage<seerep::fb::ServerResponse>();
        assert(response->Verify());

        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
      }
    }
    else
    {
      answer = "a msg had no project uuid!";
    }
  }

  // create a method for this
  flatbuffers::grpc::MessageBuilder builder;
  auto msg = builder.CreateString(answer);
  seerep::fb::ServerResponseBuilder responseBuilder(builder);
  responseBuilder.add_message(msg);
  responseBuilder.add_transmission_state(seerep::fb::TRANSMISSION_STATE_SUCCESS);
  auto responseOffset = responseBuilder.Finish();
  builder.Finish(responseOffset);
  *response = builder.ReleaseMessage<seerep::fb::ServerResponse>();
  assert(response->Verify());

  return grpc::Status::OK;
}

grpc::Status FbImageService::AddBoundingBoxes2dLabeled(
    grpc::ServerContext* context,
    grpc::ServerReader<flatbuffers::grpc::Message<seerep::fb::BoundingBoxes2DLabeledStamped>>* reader,
    flatbuffers::grpc::Message<seerep::fb::ServerResponse>* response)
{
  (void)context;  // ignore that variable without causing warnings
  std::string answer = "everything stored!";

  flatbuffers::grpc::Message<seerep::fb::BoundingBoxes2DLabeledStamped> bbsMsg;
  while (reader->Read(&bbsMsg))
  {
    BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::info) << "received BoundingBoxes2DLabeledStamped... ";
    auto bbslabeled = bbsMsg.GetRoot();

    std::string uuidProject = bbslabeled->header()->uuid_project()->str();
    if (!uuidProject.empty())
    {
      try
      {
        imageFb->addBoundingBoxesLabeled(*bbslabeled);
      }
      catch (std::runtime_error const& e)
      {
        // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
        // also catching core doesn't have project with uuid error
        BOOST_LOG_SEV(m_logger, boost::log::trivial::severity_level::error) << e.what();
        // create a method for this
        flatbuffers::grpc::MessageBuilder builder;
        auto msg = builder.CreateString(std::string(e.what()));
        seerep::fb::ServerResponseBuilder responseBuilder(builder);
        responseBuilder.add_message(msg);
        responseBuilder.add_transmission_state(seerep::fb::TRANSMISSION_STATE_FAILURE);
        auto responseOffset = responseBuilder.Finish();
        builder.Finish(responseOffset);
        *response = builder.ReleaseMessage<seerep::fb::ServerResponse>();
        assert(response->Verify());

        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
      }
    }
    else
    {
      answer = "a msg had no project uuid!";
    }
  }
  // create a method for this
  flatbuffers::grpc::MessageBuilder builder;
  auto msg = builder.CreateString(answer);
  seerep::fb::ServerResponseBuilder responseBuilder(builder);
  responseBuilder.add_message(msg);
  responseBuilder.add_transmission_state(seerep::fb::TRANSMISSION_STATE_SUCCESS);
  auto responseOffset = responseBuilder.Finish();
  builder.Finish(responseOffset);
  *response = builder.ReleaseMessage<seerep::fb::ServerResponse>();
  assert(response->Verify());

  return grpc::Status::OK;
}

} /* namespace seerep_server */
