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

int tickTime = 250;

std::string ServerAddress = "67.58.229.227";
std::string MasterAddress = "67.58.229.227";
std::mutex COMMAND_QUEUE_MUTEX;

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

volatile int thisTest = 0;

void getJustTheSeed_PepeBustEmoji(std::string host, std::string port, std::string name)
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

        auto serverFetch = [&](const char* cmd) {
            ws.write(net::buffer(std::string(cmd) + "|{}"));
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::string s = beast::buffers_to_string(buffer.data());
            return s;
        };

        std::string fetchedSeed = serverFetch("getSeed");
        World::generateSeed = false;
        World::worldSeed = std::stol(fetchedSeed);

        ws.close(websocket::close_code::normal);
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

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

        auto serverFetch = [&](const char* cmd) {
            ws.write(net::buffer(std::string(cmd) + "|{}"));
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::string s = beast::buffers_to_string(buffer.data());
            return s;
        };

        auto serverFetchWithArg = [&](const char* cmd, const char* arg) {
            ws.write(net::buffer(std::string(std::string(std::string(cmd) + "|{") + arg)+"}"));
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::string s = beast::buffers_to_string(buffer.data());
            return s;
        };

        /*std::string amIResult = serverFetchWithArg("amI", std::string(std::string("\"name\":\"") + name + "\"").c_str());

        if (*amIResult.begin() != 'n')
        {
            json myPos = json::parse(amIResult);
            GLWrapper::instance->cameraPos = glm::vec3(
                std::stof(myPos["x"].get<std::string>()),
                std::stof(myPos["y"].get<std::string>()),
                std::stof(myPos["z"].get<std::string>())
            );
        }*/


        //The infinite loop
        for (;;) {
            //std::cout << Game::instance->otherPlayersIfMultiplayer.size() << std::endl;



            std::this_thread::sleep_for(std::chrono::milliseconds(tickTime));
            std::string command;
            //inlined to guarantee to get this shit in whack
                if (prevPos != GLWrapper::instance->cameraPos)
                {
                    prevPos = GLWrapper::instance->cameraPos;
                    command = "pUp";
                }
                else {
                    command = "getUp";
                }



                json payload;

                if (command == "pUp")
                {
                    payload["x"] = std::to_string(GLWrapper::instance->cameraPos.x);
                    payload["y"] = std::to_string(GLWrapper::instance->cameraPos.y);
                    payload["z"] = std::to_string(GLWrapper::instance->cameraPos.z);
                }

                payload["name"] = name;
                std::string text = command + std::string("|") + payload.dump();

                ws.write(net::buffer(std::string(text)));

                beast::flat_buffer buffer;

                ws.read(buffer);

                std::string s = beast::buffers_to_string(buffer.data());

                json response = json::parse(s);

                auto receivePlayers = [&]() {

                    for (auto& player : response.items())
                    {
                        json thisPlayer = json::parse(player.value().get<std::string>());

                        //std::cout << thisPlayer.dump() << std::endl;

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
            
                if (*command.begin() == 'p') //pUp
                {
                    receivePlayers();
                

                }
                else
                    if (*command.begin() == 'g') //getUp
                    {
                        receivePlayers();
                    }
                    else
                        if (*command.begin() == 'a') //amI
                        {
                            if (*s.begin() == 'n')
                            {
                                //youre joining first time
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
                Game::instance->commsTimer = 0;
            
        }
        //End infinite loop





        ws.close(websocket::close_code::normal);
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

