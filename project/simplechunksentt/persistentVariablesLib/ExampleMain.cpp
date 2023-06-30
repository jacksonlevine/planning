#include "pvarslib.hpp"
#include <stdlib.h>
#include <iostream>
#include <boost/asio/ip/address.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

int main()
{
    
    PVarsContext::tableName = "jlvariables";

    ListResult all = getDbTable(PVarsContext::tableName);

    for (std::string entry : all.values())
    {
        std::cout << entry << std::endl;
    }

    return 0;
}