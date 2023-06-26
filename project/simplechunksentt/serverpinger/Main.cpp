
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


std::string MyName;
int MyPopulation = 0;
std::string MyIP;
unsigned short MyPort = 32851;

std::string Masterhost = "192.168.1.131";   //THIS WILL NEED TO BE THE PUBLIC IP AND PORT FORWARDED 


std::string getMyPublicIP(net::io_context& io_context)
{
    try {
        // Create an io_context
       net::io_context io_context;

        // Create a TCP resolver and query
        net::ip::tcp::resolver resolver(io_context);
        net::ip::tcp::resolver::query query("icanhazip.com", "80");

        // Resolve the query and get the endpoint iterator
        net::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Create a TCP socket and connect to the endpoint
        net::ip::tcp::socket socket(io_context);
        net::connect(socket, endpoint_iterator);

        // Create an HTTP request
        http::request<http::empty_body> request(http::verb::get, "/", 11);
        request.set(http::field::host, "icanhazip.com");
        request.set(http::field::user_agent, "Boost Beast");

        // Send the HTTP request
        http::write(socket, request);

        // Read the HTTP response
        beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(socket, buffer, response);

        // Get the response body as a string
        std::string responseBody = response.body();


        // Close the socket
        socket.shutdown(net::ip::tcp::socket::shutdown_both);
        socket.close();
        return responseBody;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return std::string("Error");
    }
}


std::string makeMessageForMasterServer(const char* name, int population, const char* ip, std::string cmdType)
{
    json data;
    data["name"] = name;
    data["pop"] = std::to_string(population);
    data["ip"] = ip;
    data["port"] = MyPort;

    std::string commandType = cmdType + "|";

    std::string text = commandType + data.dump();
    return text;
}

void contactMasterServer(std::string host)
{
    try
    {
        auto const Masterport = "32852";

        std::string text = makeMessageForMasterServer(MyName.c_str(), MyPopulation, MyIP.c_str(), std::string("serverUp"));

        net::io_context ioc;

        tcp::resolver resolver{ ioc };
        websocket::stream<tcp::socket> ws{ ioc };

        auto const results = resolver.resolve(host.c_str(), Masterport);

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
    }
}

int main(int argc, char** argv)
{

    std::cout << "Use default port? (Y / N)" << std::endl;
    std::string defportyorn;
    std::getline(std::cin, defportyorn);

    if (defportyorn.front() != 'y' && defportyorn.front() != 'Y')
    {
        std::cout << "Please enter an available port:" << std::endl;
        std::string portin;
        std::getline(std::cin, portin);

        unsigned short custPort = std::stoi(portin);
        MyPort = custPort;

    }

    std::cout << "Make this server public? (List it on the public server list) (Y / N)" << std::endl;
    std::string pubyorn;
    std::getline(std::cin, pubyorn);





    std::string host;
    try {
        boost::asio::io_context io_context;
        MyIP =  getMyPublicIP(io_context);
        while (!MyIP.empty() && MyIP.back() == '\n')
        {
            MyIP.pop_back();
        }
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");

        boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end; // end marker

        while (iter != end) {
            boost::asio::ip::tcp::endpoint endpoint = *iter++;
            if (endpoint.protocol() == boost::asio::ip::tcp::v4()) {
                //std::cout << "IPv4 Address: " << endpoint.address().to_string() << std::endl;
                host = endpoint.address().to_string();
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    if (pubyorn.front() == 'y' || pubyorn.front() == 'Y')
    {


        std::cout << "What name do you want your server to be listed as? (e.g. Bartholomew's Building Server)" << std::endl;
        std::getline(std::cin, MyName);

        std::cout << "Contacting the master server to list your public server..." << std::endl;
        contactMasterServer(Masterhost);
    }



  /*      try
    {
        auto const address = net::ip::make_address(host);
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
    }*/

    //THIS IS  A LIE, ITS NOT STARTING A SERVER YET
    std::string successStatement("Server now running at " + MyIP + ":" + std::to_string(MyPort));
    std::cout << successStatement;
    
    return EXIT_SUCCESS;
}