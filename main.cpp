#include <iostream>
#include "util/log.h"
#include "util/net.hpp"
#include "util/thread.h"
#include "comm.hpp"
#include "packet_merger.hpp"
using namespace std;


int main() {
    ScSocket socket;
    startServerAndConnect(socket);
    return 0;
}