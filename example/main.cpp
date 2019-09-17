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
      std::cout << "\tsearch[ " << req.url_search << " ]" << std::endl;
    });

    server->requestMapping(jshttpserver::METHOD_ALL, "/api/test2/{v1}/{v2}", [](jshttpserver::HttpRequest &req, jshttpserver::HttpResponse &res) {
      std::cout << "requestMapping FOUND :: /api/test2/{v1}/{v2}" << std::endl;
      std::cout << "\tsearch[ " << req.url_search << " ]" << std::endl;
    });

    server->requestMapping(jshttpserver::METHOD_ALL, "/api/test3/{v3:.+}", [](jshttpserver::HttpRequest &req, jshttpserver::HttpResponse &res) {
      std::cout << "requestMapping FOUND :: /api/test3/{v3:.+}" << std::endl;
      std::cout << "\tsearch[ " << req.url_search << " ]" << std::endl;
    });

    server->documentRootMapping("/docs/", [](jshttpserver::HttpRequest &req, jshttpserver::HttpResponse &res) {
      std::cout << "documentRootMapping FOUND :: /docs/ :: path = " << req.doc_path_name << std::endl;
      std::cout << "\tpath_name[ " << req.doc_path_name << " ], search[ " << req.url_search << " ]" << std::endl;
    });

    server->documentRootMapping("/doxs", [](jshttpserver::HttpRequest &req, jshttpserver::HttpResponse &res) {
      std::cout << "documentRootMapping FOUND :: /doxs :: path = " << req.doc_path_name << std::endl;
      std::cout << "\tpath_name[ " << req.doc_path_name << " ], search[ " << req.url_search << " ]" << std::endl;
    });

    server->addListen(8887);

    loop->run();
    return 0;
}
