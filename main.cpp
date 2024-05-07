#include <iostream>
#include "util/log.h"
#include "util/net.hpp"
#include "comm.hpp"
using namespace std;


int main() {

    ScSocket socket;
    startServerAndConnect(socket);
    return 0;
}