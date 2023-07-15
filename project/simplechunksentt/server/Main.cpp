
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <glm/glm.hpp>
#include <boost/bind.hpp>
#include "pvarslib.hpp"
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

const std::string ANSI_RESET = "\033[0m";
const std::string ANSI_RED = "\033[31m";
const std::string ANSI_GREEN = "\033[32m";
const std::string ANSI_YELLOW = "\033[33m";
const std::string ANSI_BLUE = "\033[34m";


std::string MyName;
int MyPopulation = 0;
std::string MyIP;
unsigned short MyPort = 32851;

std::string Masterhost = "192.168.3.30";   //THIS WILL NEED TO BE THE PUBLIC IP AND PORT FORWARDED 

//std::string Masterhost = "10.0.0.21";

//THA SEED, store this in db

long worldSeed = 0;



/// <summary>
/// The Player struct to store an individual players state
/// </summary>
class Player
{
public:
    std::string name;
    glm::vec3 pos;
    std::vector<std::string> seenBy;
};

/// <summary>
/// THE PLAYER LIST
/// List of all players currently connected to the server. Lock PLAYER_LIST_MUTEX if you want to do an operation on this.
/// </summary>
std::vector<Player> THE_PLAYER_LIST;

/// <summary>
/// LOCK THIS MUTEX before operating on THE_PLAYER_LIST
/// </summary>
std::mutex PLAYER_LIST_MUTEX;







std::time_t getTime()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration = now.time_since_epoch();
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    std::time_t time = seconds.count();
    return time;
}

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
    data["time"] = std::to_string(getTime());

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

        std::cout << ANSI_BLUE << beast::make_printable(buffer.data()) << ANSI_RESET << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void tick(const boost::system::error_code&, boost::asio::deadline_timer* t)
{
    // This is your function that should be executed every 30 minutes.
    std::cout << "Tick!" << std::endl;
    contactMasterServer(Masterhost);

    // Reschedule the timer for 30 minutes in the future.
    t->expires_at(t->expires_at() + boost::posix_time::minutes(1));

    // Post the timer event back onto the IO service.
    t->async_wait(boost::bind(tick, boost::asio::placeholders::error, t));
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
/// <summary>
/// ALWAYS LOCK THE PLAYER_LIST_MUTEX BEFORE CALLING THIS
/// </summary>
std::string getJsonifiedListOfUpdatedPlayers(const char* pName)
{
    json resp; //effectively a list of player structs filtered to only what you havent seen yet
    int index = 0;
    for (Player& i : THE_PLAYER_LIST)
    {
        if (std::find(i.seenBy.begin(), i.seenBy.end(), pName) == i.seenBy.end()) //if they havent seen this one
        {
            json j;

       
            i.seenBy.push_back(pName);

            j["name"] = i.name;
            j["x"] = i.pos.x;
            j["y"] = i.pos.y;
            j["z"] = i.pos.z;

            resp[std::to_string(index)] = j.dump();
            index++;
        }


    }
    return resp.dump();
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

            ws.read(buffer); // WAIT FOR MESSAGE


            std::string buff = beast::buffers_to_string(buffer.data());
            std::pair<std::string, std::string> result = splitString(buff);

            std::string command = result.first;
            std::string jsonno = result.second;
            std::cout << "Command: ";
            std::cout << command << std::endl;

            std::cout << "JSON: ";
            std::cout << jsonno << std::endl;




            if (command == "pUp")
            {
                /*
                * THE RECEIVED JSON PAYLOAD WILL BE:
                * 
                * name: string
                * x: float
                * y: float
                * z: float
                * 
                * 
                */



                json j = json::parse(jsonno);
                std::string pName = j["name"];

                /*LOCK MUTEX*/ std::lock_guard<std::mutex> LOCK_PLAYER_LIST(PLAYER_LIST_MUTEX);

                auto it = std::find_if(THE_PLAYER_LIST.begin(), THE_PLAYER_LIST.end(),
                    [&](const Player& p) { return p.name == pName; });

                if (it == THE_PLAYER_LIST.end())  //If theyre not in the list already
                {
                    Player newPlayer;
                    newPlayer.name = pName;
                    newPlayer.pos = glm::vec3(
                        std::stof(j["x"].get<std::string>()), 
                        std::stof(j["y"].get<std::string>()),
                        std::stof(j["z"].get<std::string>())
                    );
                    newPlayer.seenBy.push_back(pName);//and its only seen by them (its new)

                    THE_PLAYER_LIST.push_back(newPlayer);  //Make a new player struct and add it to the list
                }
                else {                            //If theyre in the list already (update their fields)
                    it->pos = glm::vec3(
                        std::stof(j["x"].get<std::string>()),
                        std::stof(j["y"].get<std::string>()),
                        std::stof(j["z"].get<std::string>())
                    );
                    it->seenBy.clear();           //Wipe the seenBy and add only them
                    it->seenBy.push_back(pName);  //This is only seen by them
                }

                //and send update info in response
                ws.write(net::buffer(getJsonifiedListOfUpdatedPlayers(pName.c_str())));



            }
            if (command == "getUp")  //They just want updates and have no updates to give
            {
                /*
                * THE RECEIVED JSON PAYLOAD WILL BE:
                *
                * name: string
                * 
                *
                *
                */

                json j = json::parse(jsonno);
                std::string pName = j["name"];

                /*LOCK MUTEX*/ std::lock_guard<std::mutex> LOCK_PLAYER_LIST(PLAYER_LIST_MUTEX);

                //and send update info in response
                ws.write(net::buffer(getJsonifiedListOfUpdatedPlayers(pName.c_str())));
            }
            if (command == "amI")  //They want to know if they were here before and load in prev pos if so
            {
                /*
                * THE RECEIVED JSON PAYLOAD WILL BE:
                *
                * name: string
                *
                *
                *
                */
                json j = json::parse(jsonno);
                std::string pName = j["name"];

                auto it = std::find_if(THE_PLAYER_LIST.begin(), THE_PLAYER_LIST.end(),
                    [&](const Player& p) { return p.name == pName; });
                if (it == THE_PLAYER_LIST.end()) //if theyre not there say no
                {
                    ws.write(net::buffer("no"));

                }
                else {                           // or else respond with payload
                    json resp;
                    resp["x"] = std::to_string(it->pos.x);
                    resp["y"] = std::to_string(it->pos.y);
                    resp["z"] = std::to_string(it->pos.z);
                    ws.write(net::buffer(resp.dump()));
                }

            }
            if (command == "getSeed")
            {
                ws.write(net::buffer(std::to_string(worldSeed)));
            }
            /*LOCK MUTEX*/ std::lock_guard<std::mutex> LOCK_PLAYER_LIST(PLAYER_LIST_MUTEX);
            MyPopulation = THE_PLAYER_LIST.size();
        }
    }
    catch (beast::system_error const& se)
    {
        if (se.code() != websocket::error::closed)
            std::cerr << "Beast Error: " << se.code().message() << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}









void startServerLoop(std::string host)
{
    try
    {
        auto const address = net::ip::make_address(host);
        auto const port = static_cast<unsigned short>(MyPort);

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
    }
}

int main(int argc, char** argv)
{
    PVarsContext::tableName = "serverWorld";
    //LOAD THE SEED OR MAKE SEED
    Result prevSeed = getDbVariable("seed");
    if (prevSeed.type == PVARSERROR)
    {
        std::cout << "No world save detected. Generating new seed..." << std::endl;
        srand(time(NULL));
        worldSeed = 10000 + rand() * 10000 + rand() * 100000;
        setDbVariable("seed", std::to_string(worldSeed).c_str());
    }
    else {
        worldSeed = std::stol(prevSeed.value()); 
    }






    std::cout << "Use default port 32581? (Y / N)" << std::endl;
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
    net::io_service io;

    boost::asio::io_service::work work(io);
    if (pubyorn.front() == 'y' || pubyorn.front() == 'Y')
    {


        std::cout << "What name do you want your server to be listed as? (e.g. Bartholomew's Building Server)" << std::endl;
        std::getline(std::cin, MyName);

        std::cout << "Contacting the master server to list your public server..." << std::endl;
        contactMasterServer(Masterhost);


        boost::asio::deadline_timer t(io, boost::posix_time::minutes(1));
        t.async_wait(boost::bind(tick, boost::asio::placeholders::error, &t));

        std::thread thread([&io] { io.run(); });

        thread.detach();

    }

    std::string successStatement("Server now running at " + MyIP + ":" + std::to_string(MyPort));
    std::cout << ANSI_RED << successStatement << ANSI_RESET << std::endl;
    startServerLoop(host);

    return EXIT_SUCCESS;
}