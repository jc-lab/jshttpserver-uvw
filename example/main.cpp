#include <iostream>

#include <uvw.hpp>
#include <jshttpserver/server.hpp>
#include <jshttpserver/http_request.hpp>

#pragma comment(lib, "libuv.lib")

int main() {
    auto loop = uvw::Loop::getDefault();

    std::shared_ptr<jshttpserver::Server> server(jshttpserver::Server::create(loop));

    server->requestMapping(jshttpserver::METHOD_ALL, "/api/test", [](jshttpserver::HttpRequest &req, jshttpserver::HttpResponse &res) {
      std::cout << "requestMapping FOUND :: /api/test" << std::endl;
    });

    server->requestMapping(jshttpserver::METHOD_ALL, "/api/test2/{v1}/{v2}", [](jshttpserver::HttpRequest &req, jshttpserver::HttpResponse &res) {
      std::cout << "requestMapping FOUND :: /api/test2/{v1}/{v2}" << std::endl;
    });

    server->requestMapping(jshttpserver::METHOD_POST, "/api/test3/{v3}", [](jshttpserver::HttpRequest &req, jshttpserver::HttpResponse &res) {
      std::cout << "requestMapping FOUND :: /api/test3/{v3}" << std::endl;
    });

    server->addListen(8887);

    loop->run();
    return 0;
}
