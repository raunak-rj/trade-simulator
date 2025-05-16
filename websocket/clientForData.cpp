#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <openssl/ssl.h>
#include "json/json.hpp"
#include <fstream>
using json = nlohmann::json;
typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
// TLS initialization (required)
websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>
on_tls_init(websocketpp::connection_hdl) {
    auto ctx = websocketpp::lib::make_shared<websocketpp::lib::asio::ssl::context>(
        websocketpp::lib::asio::ssl::context::sslv23);
    ctx->set_default_verify_paths();
    ctx->set_options(websocketpp::lib::asio::ssl::context::default_workarounds |
                     websocketpp::lib::asio::ssl::context::no_sslv2 |
                     websocketpp::lib::asio::ssl::context::no_sslv3 |
                     websocketpp::lib::asio::ssl::context::single_dh_use);
    return ctx;
}

void on_open(client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connected successfully!" << std::endl;
}

void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    std::string payload = msg->get_payload();

    try {
        json j = json::parse(payload);

        // Append to file (newline-delimited JSON format — NDJSON)
        std::ofstream out("data/l2_orderbook2.json", std::ios::app);
        out << j.dump() << "\n";
        out.close();

        std::cout << " Snapshot saved.\n";

    } catch (const std::exception& e) {
        std::cerr << "Failed to parse or write JSON: " << e.what() << "\n";
    }
}

void on_close(client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connection closed" << std::endl;
}

int main() {
    client c;
    
    // Initialize Asio
    c.init_asio();
    c.set_tls_init_handler(&on_tls_init);
    c.set_open_handler(std::bind(&on_open,&c,std::placeholders::_1));
    c.set_message_handler(std::bind(&on_message,&c,std::placeholders::_1,std::placeholders::_2));
    c.set_close_handler(std::bind(&on_close,&c,std::placeholders::_1));

    websocketpp::lib::error_code ec;
    auto con = c.get_connection("wss://ws.gomarket-cpp.goquant.io/ws/l2-orderbook/okx/BTC-USDT-SWAP", ec);
    if (ec) {
        std::cout << "Connection error: " << ec.message() << std::endl;
        return 1;
    }

    c.connect(con);
    c.run();
}
