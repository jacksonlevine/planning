
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

int main(int argc, char** argv)
{
    std::string host;
    try {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");

        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end; // end marker

        while (iter != end) {
            boost::asio::ip::tcp::endpoint endpoint = *iter++;
            if (endpoint.protocol() == boost::asio::ip::tcp::v4()) {
                std::cout << "IPv4 Address: " << endpoint.address().to_string() << std::endl;
                host = endpoint.address().to_string();
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    try
    {
        auto const port = "32851";


        json data;
        data["name"] = "J-man's Server";
        data["pop"] = std::to_string(20);
        data["ip"] = host;

        auto const text = data.dump();

        net::io_context ioc;

        tcp::resolver resolver{ ioc };
        websocket::stream<tcp::socket> ws{ ioc };

        auto const results = resolver.resolve(host.c_str(), port);

        net::connect(ws.next_layer(), results.begin(), results.end());

        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-client-coro");
            }));

        ws.handshake(host, "/");

        ws.write(net::buffer(std::string(text)));

        beast::flat_buffer buffer;

        ws.read(buffer);

        ws.close(websocket::close_code::normal);

        std::cout << beast::make_printable(buffer.data()) << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}