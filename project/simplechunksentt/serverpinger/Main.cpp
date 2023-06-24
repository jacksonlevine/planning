
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

int main(int argc, char** argv)
{
    try
    {
        auto const host = "127.0.0.1";
        auto const port = "32851";
        auto const text = "I'm here";

        net::io_context ioc;

        tcp::resolver resolver{ ioc };
        websocket::stream<tcp::socket> ws{ ioc };

        auto const results = resolver.resolve(host, port);

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