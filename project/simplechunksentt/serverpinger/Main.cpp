#include <iostream>
#include <boost/asio.hpp>


#define JONNA_PORT 3285

namespace asio = boost::asio;
namespace ip = boost::asio::ip;

int main()
{
    asio::io_context io_context;

    ip::tcp::socket socket(io_context);
    ip::tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), JONNA_PORT); // Replace with your server's IP address and port

    try
    {
        socket.connect(endpoint);

        std::string message = "Hello, server!";
        asio::write(socket, asio::buffer(message));

        std::cout << "Message sent to the server." << std::endl;

        socket.close();
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}