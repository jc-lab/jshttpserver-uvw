#include <iostream>

#include <uvw.hpp>
#include <jshttpserver/server.hpp>

#pragma comment(lib, "libuv.lib")

int main() {
    auto loop = uvw::Loop::getDefault();

    std::shared_ptr<jshttpserver::Server> server(jshttpserver::Server::create(loop));

    server->addListen(8887);

    loop->run();
    return 0;
}
