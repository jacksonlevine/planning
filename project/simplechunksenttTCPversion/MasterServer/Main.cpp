
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include "../persistentVariablesLib/pvarslib.hpp"
#include <chrono>

// THIS IS THE MASTER SERVER

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

std::atomic<bool> isAccessingDatabase(false);

std::time_t getTime()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration = now.time_since_epoch();
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    std::time_t time = seconds.count();
    return time;
}

std::pair<std::string, std::string> splitString(const std::string& str) {
    size_t index = str.find('|'); // Find the index of the '|' character

    if (index != std::string::npos) { // If '|' is found
        std::string before = str.substr(0, index); // Extract the string before '|'
        std::string after = str.substr(index + 1); // Extract the string after '|'
        return std::make_pair(before, after);
    }
    else {
        return std::make_pair("", ""); // Return empty strings if '|' is not found
    }
}
bool isEntryOlderThan1Minutes(const std::string& entryTime) {
    // Convert the entryTime string to a long long int
    long long int entryTimeValue = std::stoll(entryTime);

    // Convert the entryTime value to time_t
    time_t entryTimeT = static_cast<time_t>(entryTimeValue);

    // Get the current time
    time_t currentTime = std::time(nullptr);

    // Calculate the time difference in minutes
    double minutesPassed = std::difftime(currentTime, entryTimeT) / 60.0;

    // Check if the entry is older than 1 minutes
    if (minutesPassed > 1.0) {
        return true;
    }
    else {
        return false;
    }
}

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
            //ws.text(ws.got_text());
            //ws.write(buffer.data());

            
            std::string buff = beast::buffers_to_string(buffer.data());
            std::pair<std::string, std::string> result = splitString(buff);

            std::string command = result.first;
            std::string jsonno = result.second;
            std::cout << "Command: ";
            std::cout << command << std::endl;

            std::cout << "JSON: ";
            std::cout << jsonno << std::endl;

            


            if (command == "serverUp")
            {
                json j = json::parse(jsonno);
                std::string serverName = j["name"];



                while (isAccessingDatabase.load() != false)
                {
                    //Wait until database is not being accessed by another thread
                }
                isAccessingDatabase.store(true);

                //DO YOUR ACCESSING getdbvariable and setdbvariable HERE

                std::string sinfo = getDbVariable(serverName.c_str()).value();
                if (sinfo != "noexist")
                {
                    json existingInfo = json::parse(sinfo);
                    if (existingInfo["ip"] != j["ip"] || existingInfo["port"] != j["port"])  //If its the same name but different IP
                    {
                        serverName += "1";
                    }
                }

                setDbVariable(serverName.c_str(), jsonno.c_str());

                isAccessingDatabase.store(false);

                ws.write(net::buffer("Master server: Got your public server. Thanks!"));
            }
            if (command == "getAll")
            {
                while (isAccessingDatabase.load() != false)
                {
                    //Wait until database is not being accessed by another thread
                }
                isAccessingDatabase.store(true);

                ListResult serverList = getDbTable(PVarsContext::tableName);
                if (serverList.type = PVARSERROR)
                {
                    std::cerr << "I was unable to get the list of servers." << std::endl;
                }
                else {
                    json resp;
                    int index = 0;
                    for (std::string& s : serverList.values())
                    {
                        
                        json entry = json::parse(s);

                        if (isEntryOlderThan1Minutes(entry["time"]))
                        {
                            
                            deleteDbVariable(entry["name"]);
                        }
                        else {
                            resp[std::to_string(index)] = s;
                            index++;
                        }
                    }

                    ws.write(net::buffer(resp.dump()));
                }

                isAccessingDatabase.store(false);
            }

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
    PVarsContext::tableName = "masterServerList";


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
               // std::cout << "IPv4 Address: " << endpoint.address().to_string() << std::endl;
                host = endpoint.address().to_string();
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    try
    {
        auto const address = net::ip::make_address(host);
        auto const port = static_cast<unsigned short>(32852);

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