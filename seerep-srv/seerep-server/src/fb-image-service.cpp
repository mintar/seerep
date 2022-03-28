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
  auto requestRoot = request->GetRoot();
  std::cout << "sending images in bounding box min(" << requestRoot->boundingbox()->point_min()->x() << "/"
            << requestRoot->boundingbox()->point_min()->y() << "/" << requestRoot->boundingbox()->point_min()->z()
            << "), max(" << requestRoot->boundingbox()->point_max()->x() << "/"
            << requestRoot->boundingbox()->point_max()->y() << "/" << requestRoot->boundingbox()->point_max()->z()
            << ")"
            << " and time interval (" << requestRoot->timeinterval()->time_min()->seconds() << "/"
            << requestRoot->timeinterval()->time_max()->seconds() << ")" << std::endl;

  try
  {
    imageFb->getData(*requestRoot, writer);
  }
  catch (std::runtime_error e)
  {
    // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
    // also catching core doesn't have project with uuid error
    std::cout << e.what() << std::endl;
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, e.what());
  }

  return grpc::Status::OK;
}

grpc::Status FbImageService::TransferImage(grpc::ServerContext* context,
                                           grpc::ServerReader<flatbuffers::grpc::Message<seerep::fb::Image>>* reader,
                                           flatbuffers::grpc::Message<seerep::fb::ServerResponse>* response)
{
  std::cout << "received image... " << std::endl;
  std::string answer = "everything stored!";

  flatbuffers::grpc::Message<seerep::fb::Image> imageMsg;
  while (reader->Read(&imageMsg))
  {
    auto image = imageMsg.GetRoot();

    std::string uuidProject = image->header()->uuid_project()->str();
    if (!uuidProject.empty())
    {
      try
      {
        boost::uuids::uuid uuidImg = imageFb->addData(*image);
      }
      catch (std::runtime_error e)
      {
        // mainly catching "invalid uuid string" when transforming uuid_project from string to uuid
        // also catching core doesn't have project with uuid error
        std::cout << e.what() << std::endl;

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