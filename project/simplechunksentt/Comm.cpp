#pragma once

#include "Comm.hpp"
#include "../simplechunksentt/persistentVariablesLib/pvarslib.hpp"
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

void getPublicListings(std::string masterServerIp)
{
    try
    {
        auto const Masterport = "32852";

        std::string text = "getAll|{}";

        net::io_context ioc;

        tcp::resolver resolver{ ioc };
        websocket::stream<tcp::socket> ws{ ioc };

        auto const results = resolver.resolve(masterServerIp.c_str(), Masterport);

        net::connect(ws.next_layer(), results.begin(), results.end());

        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-client-coro");
            }));

        ws.handshake(masterServerIp, "/");

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

glm::vec3 prevPos;


void addServerCommandsToQueue(Game* g)
{
    //std::cout << "Adding commands!";
    //std::cout << Game::instance->serverCommandQueue.size() << std::endl;
    if (prevPos != GLWrapper::instance->cameraPos)
    {
        prevPos = GLWrapper::instance->cameraPos;
        g->instance->serverCommandQueue.push_back("pUp");
    }
    else {
        g->instance->serverCommandQueue.push_back("getUp");
    }
}

volatile int thisTest = 0;

void startTalkingToServer(std::string host, std::string port, std::string name)
{
    try
    {

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

        //The infinite loop

        for (;;) {
            std::cout << Game::instance->otherPlayersIfMultiplayer.size() << std::endl;
            bool queueEmpty = (Game::instance->serverCommandQueue.size() == 0);

            while (queueEmpty) {
                thisTest = rand();
                // Wait until the condition becomes false

                queueEmpty = (Game::instance->serverCommandQueue.size() == 0);
            }
            ///*LOCK MUTEX*/ std::lock_guard<std::mutex> lock(COMMAND_QUEUE_MUTEX);

            //The command entered
            auto command = Game::instance->serverCommandQueue.begin();

            json payload;

            if (*command == "pUp")
            {
                payload["x"] = std::to_string(GLWrapper::instance->cameraPos.x);
                payload["y"] = std::to_string(GLWrapper::instance->cameraPos.y);
                payload["z"] = std::to_string(GLWrapper::instance->cameraPos.z);
            }

            payload["name"] = name;
            std::string text = std::string(*command) + std::string("|") + payload.dump();

            ws.write(net::buffer(std::string(text)));

            beast::flat_buffer buffer;

            ws.read(buffer);

            std::string s = beast::buffers_to_string(buffer.data());

            json response = json::parse(s);

            auto receivePlayers = [&]() {

                for (auto& player : response.items())
                {
                    json thisPlayer = json::parse(player.value().get<std::string>());
                    //this is one player on the list that needs updating
                    auto it = std::find_if(Game::instance->otherPlayersIfMultiplayer.begin(), Game::instance->otherPlayersIfMultiplayer.end(), [&](const Player& p) {
                        return p.name == thisPlayer["name"];
                        });
                    if (it == Game::instance->otherPlayersIfMultiplayer.end()) //first time we heard about this player
                    {
                        Player p;
                        p.name = thisPlayer["name"];
                        p.pos = glm::vec3(
                            thisPlayer["x"].get<float>(),
                            thisPlayer["y"].get<float>(),
                            thisPlayer["z"].get<float>()
                        );
                        p.prevPos = p.pos;
                        Game::instance->otherPlayersIfMultiplayer.push_back(p);
                    }
                    else {                                                      //existing player updating
                        it->prevPos = it->pos;
                        it->pos = glm::vec3(
                            thisPlayer["x"].get<float>(),
                            thisPlayer["y"].get<float>(),
                            thisPlayer["z"].get<float>()
                        );
                    }
                }

            };
            
            if (*command == "pUp")
            {
                receivePlayers();
                

            }
            else
                if (*command == "getUp")
                {
                    receivePlayers();
                }
                else
                    if (*command == "amI")
                    {
                        if (*s.begin() == 'n')
                        {
                            //youre joining first time, no worries!
                        }
                        else {
                            //the response has where you should be
                            GLWrapper::instance->cameraPos = glm::vec3(
                                response["x"].get<float>(),
                                response["y"].get<float>(),
                                response["z"].get<float>()
                            );
                        }
                    }
            Game::instance->serverCommandQueue.erase(command); //command done, take it from queue & do next one/wait for another
        }
        //End infinite loop





        ws.close(websocket::close_code::normal);
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

