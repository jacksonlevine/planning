#pragma once
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
class Player
{
public:
    std::string name;
    glm::vec3 pos;
    std::vector<std::string> seenBy;

    glm::vec3 prevPos;
};


#include "Game.hpp"

//static std::mutex COMMAND_QUEUE_MUTEX;

void getPublicListings(std::string masterServerIp);

void startTalkingToServer(std::string host, std::string port, std::string name);

void addServerCommandsToQueue(Game* g); //Task for the game to run every x seconds
/// <summary>
/// The Player struct to store an individual players state
/// </summary>

