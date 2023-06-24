
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void do_session(tcp::socket& socket)
{
    try
    {
        websocket::stream<tcp::socket> ws{ std::move(socket) };

        ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-server-sync");
            }));

        // Accept the websocket handshake
        ws.accept();

        for (;;)
        {
            beast::flat_buffer buffer;

            ws.read(buffer);

            // Echo the message back
            ws.text(ws.got_text());
            ws.write(buffer.data());
            std::cout << beast::make_printable(buffer.data());
        }
    }
    catch (beast::system_error const& se)
    {
        if (se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        auto const address = net::ip::make_address("127.0.0.1");
        auto const port = static_cast<unsigned short>(32851);

        net::io_context ioc{ 1 };

        tcp::acceptor acceptor{ ioc, {address, port} };
        for (;;)
        {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);
            std::thread{ std::bind(
                &do_session,
                std::move(socket)) }.detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}