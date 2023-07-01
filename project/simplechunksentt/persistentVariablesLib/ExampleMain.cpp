#include "pvarslib.hpp"
#include <stdlib.h>
#include <iostream>
#include <boost/asio/ip/address.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

int main()
{
    
    PVarsContext::tableName = "jlvariables";

    deleteDbVariable("Hoopy");
    return 0;
}