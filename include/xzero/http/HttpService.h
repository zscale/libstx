#pragma once

#include <xzero/Api.h>
#include <xzero/DateTime.h> // BuiltinAssetHandler
#include <vector>
#include <unordered_map>

namespace xzero {

class Server;
class Connector;
class LocalConnector;
class InetConnector;
class HttpRequest;
class HttpResponse;
class Executor;
class Scheduler;
class Selector;
class IPAddress;

/**
 * General purpose multi-handler HTTP Service API.
 *
 * @note HTTP/1 is always enabled by default.
 */
class XZERO_API HttpService {
 public:
  class Handler;
  class BuiltinAssetHandler;

  HttpService();
  ~HttpService();

  /**
   * Configures this service to listen on TCP/IP using the given parameters.
   *
   * @param executor the executor service to run tasks on.
   * @param scheduler where to schedule timed tasks on.
   * @param selector the I/O selector service to use for non-blocking I/O.
   * @param ipaddress the TCP/IP bind address.
   * @param port the TCP/IP port to listen on.
   * @param backlog the number of connections allowed to be queued in kernel.
   *
   */
  InetConnector* configureInet(Executor* executor, Scheduler* scheduler,
                               Selector* selector, const IPAddress& ipaddress,
                               int port, int backlog = 128);

  /** Configures a local connector. */
  LocalConnector* configureLocal();

  /** Registers a new @p handler. */
  void addHandler(Handler* handler);

  /** Removes given @p handler from the list of registered handlers. */
  void removeHandler(Handler* handler);

  /** Starts the internal server. */
  void start();

  /** Stops the internal server. */
  void stop();

 private:
  void enableHttp1(Connector* connector);
  void handleRequest(HttpRequest* request, HttpResponse* response);

 private:
  Server* server_;
  LocalConnector* localConnector_;
  InetConnector* inetConnector_;
  std::vector<Handler*> handlers_;
};

/**
 * Interface for general purpose HTTP request handlers.
 */
class XZERO_API HttpService::Handler {
 public:
  /**
   * Attempts to handle the given request.
   *
   * @retval true the request is being handled.
   * @retval false the request is not being handled.
   */
  virtual bool handleRequest(HttpRequest* request, HttpResponse* response) = 0;
};

/**
 * Builtin Asset Handler for HttpService.
 */
class XZERO_API HttpService::BuiltinAssetHandler : public Handler {
 public:
  BuiltinAssetHandler();

  void addAsset(const std::string& path, const std::string& mimetype,
                const Buffer&& data);

  bool handleRequest(HttpRequest* request, HttpResponse* response) override;

 private:
  struct Asset {
    std::string mimetype;
    DateTime mtime;
    Buffer data;
  };

  std::unordered_map<std::string, Asset> assets_;
};

} // namespace xzero
