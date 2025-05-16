#include "webSocketClient.h"
#include <openssl/ssl.h>
#include <iostream>
#include <thread>

webSocketClient::webSocketClient() : latencyLogging(false)
{
    client.init_asio();

    client.set_tls_init_handler(
        std::bind(&webSocketClient::on_tls_init, this, std::placeholders::_1));
    client.set_open_handler(
        std::bind(&webSocketClient::on_open, this, std::placeholders::_1));
    client.set_message_handler(
        std::bind(&webSocketClient::on_message, this, std::placeholders::_1, std::placeholders::_2));
    client.set_close_handler(
        std::bind(&webSocketClient::on_close, this, std::placeholders::_1));
}

websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>
webSocketClient::on_tls_init(websocketpp::connection_hdl)
{
    auto ctx = websocketpp::lib::make_shared<websocketpp::lib::asio::ssl::context>(
        websocketpp::lib::asio::ssl::context::sslv23);
    ctx->set_default_verify_paths();
    ctx->set_options(
        websocketpp::lib::asio::ssl::context::default_workarounds |
        websocketpp::lib::asio::ssl::context::no_sslv2 |
        websocketpp::lib::asio::ssl::context::no_sslv3 |
        websocketpp::lib::asio::ssl::context::single_dh_use);
    return ctx;
}

void webSocketClient::on_open(websocketpp::connection_hdl)
{
    std::cout << "Connected to websocket \n";
}

void webSocketClient::on_message(websocketpp::connection_hdl,
                                 WSClient::message_ptr msg)
{
    auto start = std::chrono::steady_clock::now();

    if (userCallback)
        userCallback(msg->get_payload());

    if (latencyLogging)
    {
        auto end = std::chrono::steady_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        // std::cout << "Tick latency: " << latency << " µs\n";
    }
}

void webSocketClient::on_close(websocketpp::connection_hdl)
{
    std::cout << "Connection closed.\n";
}

void webSocketClient::connect(const std::string &uri)
{
    lastUri = uri;
    websocketpp::lib::error_code ec;
    auto con = client.get_connection(uri, ec);

    if (ec)
    {
        std::cerr << "Connection error: " << ec.message() << "\n";
        return;
    }

    client.connect(con);
    client.run();
}

void webSocketClient::connectAsync(const std::string &uri)
{
    lastUri = uri;
    websocketpp::lib::error_code ec;
    auto con = client.get_connection(uri, ec);

    if (ec)
    {
        std::cerr << "Connection error: " << ec.message() << "\n";
        return;
    }

    client.connect(con);

    std::thread t([this]()
                  { client.run(); });
    t.detach();
}

void webSocketClient::setMessageCallback(std::function<void(const std::string &)> callback)
{
    userCallback = callback;
}

void webSocketClient::enableLatencyLogging(bool enable)
{
    latencyLogging = enable;
}
