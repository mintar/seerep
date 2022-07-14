#ifndef SEEREP_SERVER_SERVER_H_
#define SEEREP_SERVER_SERVER_H_

#include <signal.h>

#include <fstream>

// grpc
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

// seerep
#include <seerep-core/core.h>

#include "seerep-server/fb-image-service.h"
#include "seerep-server/fb-instance-service.h"
#include "seerep-server/fb-meta-operations.h"
#include "seerep-server/fb-tf-service.h"
#include "seerep-server/pb-image-service.h"
#include "seerep-server/pb-meta-operations.h"
#include "seerep-server/pb-point-cloud-service.h"
#include "seerep-server/pb-tf-service.h"

// logging
#include <boost/log/core.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

// program options / cmd line args
#include <boost/program_options.hpp>

namespace seerep_server
{
/**
 * @brief This class is the main class of SEEREP. It creates everything else and serves the gRPC interface.
 *
 * It initialises the logging and parses the configuration from command line or cfg file.
 *
 * The various flatbuffer and protobuf services are created here and served via the server.
 */
class server
{
public:
  /**
   * @brief Constructs the services and the server
   * @param argc argument count of the command line arguments
   * @param argv argument vector of the command line arguments
   */
  server(int argc, char** argv);
  /**
   * @brief serves the services and waits for queries
   */
  void serve();

private:
  /**
   * @brief signal handler to flush the logging to file on SIGTERM
   */
  static void signalHandler(int signum);
  /**
   * @brief parses the command line arguments
   * @param argc argument count of the command line arguments
   * @param argv argument vector of the command line arguments
   */
  void parseProgramOptions(int argc, char** argv);
  /**
   * @brief initializes the logging
   *
   * Logs are stored in a log file and streamed to stdout
   */
  void initLogging();

  /**
   * @brief creates the gRPC server
   *
   * The SEEREP core, the flatbuffer services and the protobuf services are created
   * and used to create the server
   */
  void createGrpcServer();
  /**
   * @brief Returns the path to the data folder. The folder is created if not existing
   * @return path to the data folder
   */
  std::string getDataFolder();

  /**
   * @brief calls the create function and registers the protobuf services at the server
   * @param server_builder the ServerBuilder which is used to build the server and needed to add the services
   */
  void addServicesPb(grpc::ServerBuilder& server_builder);
  /**
   * @brief calls the create function and registers the flatbuffer services at the server
   * @param server_builder the ServerBuilder which is used to build the server and needed to add the services
   */
  void addServicesFb(grpc::ServerBuilder& server_builder);

  /**
   * @brief creates the protobuf services
   */
  void createServicesPb();
  /**
   * @brief creates the flatbuffer services
   */
  void createServicesFb();

  /** @brief the value map of the program options from commandline or cfg file*/
  // Abkürzung von dem Parameter ist im QuellCode nicht mehr verständlich
  boost::program_options::variables_map m_vm;

  /** @brief the seerep core which deals with the data indexing*/
  std::shared_ptr<seerep_core::Core> m_seerepCore;
  /** @brief the gRPC server which serves the gRPC services*/
  std::shared_ptr<grpc::Server> m_grpcServer;

  /** @brief the protobuf service for meta operations*/
  std::shared_ptr<seerep_server::PbMetaOperations> m_metaOperationsPb;
  /** @brief the protobuf service for transformation related queries*/
  std::shared_ptr<seerep_server::PbTfService> m_tfServicePb;
  /** @brief the protobuf service for image related queries*/
  std::shared_ptr<seerep_server::PbImageService> m_imageServicePb;
  /** @brief the protobuf service for point cloud related queries*/
  std::shared_ptr<seerep_server::PbPointCloudService> m_pointCloudServicePb;

  /** @brief the flatbuffer service for meta operations*/
  std::shared_ptr<seerep_server::FbMetaOperations> m_metaOperationsFb;
  /** @brief the flatbuffer service for transformation related queries*/
  std::shared_ptr<seerep_server::FbTfService> m_tfServiceFb;
  /** @brief the flatbuffer service for image related queries*/
  std::shared_ptr<seerep_server::FbImageService> m_imageServiceFb;
  /** @brief the flatbuffer service for instances related queries*/
  std::shared_ptr<seerep_server::FbInstanceService> m_instanceServiceFb;

  /** @brief the logger object for logging to file and stdout*/
  boost::log::sources::severity_logger<boost::log::trivial::severity_level> m_logger;
};
} /* namespace seerep_server */
#endif  // SEEREP_SERVER_SERVER_H_
