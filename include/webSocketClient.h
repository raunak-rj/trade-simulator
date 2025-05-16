#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <functional>
#include <string>
#include <chrono>

typedef websocketpp::client<websocketpp::config::asio_tls_client> WSClient;

class webSocketClient {
public:
    webSocketClient();

    // Connect to URI (blocking)
    void connect(const std::string& uri);

    // Connect and run async (non-blocking)
    void connectAsync(const std::string& uri);

    // External callback handler for payload processing
    void setMessageCallback(std::function<void(const std::string&)> callback);

    // Enable latency logging per message
    void enableLatencyLogging(bool enable);

private:
    WSClient client;
    std::function<void(const std::string&)> userCallback;
    std::string lastUri;
    bool latencyLogging;

    // TLS setup
    websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>
    on_tls_init(websocketpp::connection_hdl);

    // Event handlers
    void on_open(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, WSClient::message_ptr msg);
    void on_close(websocketpp::connection_hdl hdl);
};

#endif
