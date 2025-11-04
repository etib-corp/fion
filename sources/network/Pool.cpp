#include "network/Pool.hpp"
#include "Handler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "logging/Logger.hpp"
#include <iostream>
#include <sstream>

namespace fion::network {
Pool::Pool(Router *router) : _router(router) {
  // Set up the event callback
  _loop.set_event_callback(
      [this](int fd, uint32_t events) { handle_client_event(fd, events); });
}

Pool::~Pool() { stop(); }

void Pool::run() {
  logging::Logger::info("Pool: starting event loop thread");
  _thread = std::thread([this]() { _loop.run(); });
}

void Pool::stop() {
  logging::Logger::info("Pool: stopping event loop thread");
  _loop.stop();
  if (_thread.joinable())
    _thread.join();
  logging::Logger::info("Pool: event loop thread stopped");
}

void Pool::addClient(int fd) {
  _connectionPool.addClient(fd);

  // Add the client socket to the event loop for reading
  uint32_t events = static_cast<uint32_t>(PollerEvent::READ) |
                    static_cast<uint32_t>(PollerEvent::EDGE_TRIGGERED);
  _loop.get_poller().addFD(fd, events);
  logging::Logger::debug("Pool: added client fd=" + std::to_string(fd) +
                         ", events=READ|EDGE");
}

void Pool::handle_client_event(int fd, uint32_t events) {
  Client *client = _connectionPool.getClient(fd);
  if (!client)
    return;

  // Handle errors and hangup
  if (events & (static_cast<uint32_t>(PollerEvent::ERROR) |
                static_cast<uint32_t>(PollerEvent::HANGUP))) {
    logging::Logger::warning("Pool: client fd=" + std::to_string(fd) +
                             " error/hangup; closing");
    _loop.get_poller().removeFD(fd);
    _connectionPool.removeClient(fd);
    return;
  }

  // Handle read events
  if (events & static_cast<uint32_t>(PollerEvent::READ)) {
    ssize_t bytes_read = client->readRequest();
    logging::Logger::debug("Pool: fd=" + std::to_string(fd) +
                           " read bytes=" + std::to_string(bytes_read));

    if (bytes_read <= 0) {
      // Connection closed or error
      logging::Logger::debug("Pool: fd=" + std::to_string(fd) +
                             " read <= 0; closing");
      _loop.get_poller().removeFD(fd);
      _connectionPool.removeClient(fd);
      return;
    }

    // Check if request is complete
    if (client->is_request_ready()) {
      logging::Logger::debug("Pool: fd=" + std::to_string(fd) +
                             " request ready; processing");
      client->set_state(ClientState::PROCESSING);
      process_request(client);

      // Try to write immediately
      ssize_t bytes_sent = client->writeResponse();
      logging::Logger::debug(
          "Pool: fd=" + std::to_string(fd) +
          " immediate write bytes=" + std::to_string(bytes_sent));

      if (bytes_sent < 0) {
        // Error sending
        logging::Logger::error("Pool: fd=" + std::to_string(fd) +
                               " write error; closing");
        _loop.get_poller().removeFD(fd);
        _connectionPool.removeClient(fd);
        return;
      }

      // Response sent, close connection (HTTP/1.0 style for now)
      logging::Logger::debug("Pool: fd=" + std::to_string(fd) +
                             " response sent; closing connection");
      _loop.get_poller().removeFD(fd);
      _connectionPool.removeClient(fd);
    } else {
      logging::Logger::debug("Pool: fd=" + std::to_string(fd) +
                             " request incomplete; waiting for more data");
    }
  }

  // Handle write events (if we ever need them for async writes)
  // For now, we write immediately after processing
}

void Pool::process_request(Client *client) {
  try {
    auto request_data = std::string(client->get_request_data());

    // Parse HTTP request (simplified parsing)
    // Split into start line, headers, and body
    size_t first_crlf = request_data.find("\r\n");
    if (first_crlf == std::string::npos) {
      // Invalid request
      http::Response response;
      response.setStatusCode(http::StatusCode::BAD_REQUEST);
      response.setBody("Bad Request");
      client->prepare_response(response);
      logging::Logger::warning("Pool: invalid request (no start line)");
      return;
    }

    std::string start_line = request_data.substr(0, first_crlf);

    size_t headers_end = request_data.find("\r\n\r\n");
    if (headers_end == std::string::npos) {
      http::Response response;
      response.setStatusCode(http::StatusCode::BAD_REQUEST);
      response.setBody("Bad Request");
      client->prepare_response(response);
      logging::Logger::warning("Pool: invalid request (no headers end)");
      return;
    }

    std::string headers =
        request_data.substr(first_crlf + 2, headers_end - first_crlf - 2);
    std::string body = request_data.substr(headers_end + 4);

    // Create request object
    http::Request request(start_line, headers, body);

    // Route to handler
    auto url = request.getURL();
    std::string path = url.getPathToResource();

    // Convert method to string for routing
    std::string method;
    switch (request.getMethod()) {
    case http::Method::GET:
      method = "GET";
      break;
    case http::Method::POST:
      method = "POST";
      break;
    case http::Method::PUT:
      method = "PUT";
      break;
    case http::Method::DELETE:
      method = "DELETE";
      break;
    default:
      method = "GET";
      break;
    }

    logging::Logger::info("Pool: routing " + method + " " + path);

    auto handler = _router->findRoute(path, method);

    if (handler) {
      // Execute handler
      auto response = handler->handle(std::make_unique<http::Request>(request));
      // Ensure a basic Connection header (close) to simplify lifecycle
      response->setHeader("Connection", "close");
      client->prepare_response(*response);
      logging::Logger::debug("Pool: handler produced response");
    } else {
      // No handler found, return 404
      http::Response response;
      response.setStatusCode(http::StatusCode::NOT_FOUND);
      response.setBody("Not Found");
      response.setHeader("Connection", "close");
      client->prepare_response(response);
      logging::Logger::info("Pool: no route found for " + method + " " + path);
    }
  } catch (const std::exception &e) {
    // Error processing request
    http::Response response;
    response.setStatusCode(http::StatusCode::INTERNAL_SERVER_ERROR);
    response.setBody("Internal Server Error");
    response.setHeader("Connection", "close");
    client->prepare_response(response);
    logging::Logger::error(std::string("Pool: exception during processing: ") +
                           e.what());
  }
}

} // namespace fion::network
