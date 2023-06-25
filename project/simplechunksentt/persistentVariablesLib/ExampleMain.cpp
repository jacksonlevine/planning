#include "pvarslib.hpp"
#include <stdlib.h>
#include <iostream>
#include <boost/asio/ip/address.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

int main()
{
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
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}