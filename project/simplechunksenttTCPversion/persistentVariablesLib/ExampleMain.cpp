#include "pvarslib.hpp"
#include <stdlib.h>
#include <iostream>
#include <boost/asio/ip/address.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

int main()
{

        std::cout << "Printable ASCII [32..126]:\n";
        for (char c{ ' ' }; c <= '~'; ++c)
            std::cout << c << ((c + 1) % 32 ? ' ' : '\n');
        std::cout << '\n';

    return 0;
}