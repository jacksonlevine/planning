#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>

#define JONNA_PORT 3285

auto handle_connection = [](boost::asio::ip::tcp::socket sock) {
    std::array<char, 1024> buffer; 

    sock.async_read_some(boost::asio::buffer(buffer), [&](boost::system::error_code ec, std::size_t bytes_transferred) {
        if (!ec)
        {
            std::cout << "Received data: " << std::string(buffer.data(), bytes_transferred) << std::endl;
        }
        else
        {
            std::cout << "Error code: " << ec.what() << std::endl;
        }
        });

};

void start_accepting(boost::asio::ip::tcp::acceptor& acceptor)
{
    acceptor.async_accept([&](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec)
        {
            handle_connection(std::move(socket));
        }

        start_accepting(acceptor);
        });
}


int main()
{
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),JONNA_PORT));

    acceptor.async_accept([&](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec)
        {
            handle_connection(std::move(socket));
        }
        start_accepting(acceptor);
        });

    io_context.run();

	return 0;
}