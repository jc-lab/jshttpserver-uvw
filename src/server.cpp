/**
 * @file	server.cpp
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/ )
 * @date    2019/09/17
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <jshttpserver/server.hpp>
#include <jshttpserver/client.hpp>

#include <jshttpserver/http_request.hpp>
#include <jshttpserver/http_response.hpp>

#include <iostream>

namespace jshttpserver {
    std::shared_ptr<Server> Server::create(std::shared_ptr<uvw::Loop> loop) {
        std::shared_ptr<Server> instance(new Server(loop));
        return instance;
    }

    Server::Server(std::shared_ptr<uvw::Loop> loop)
        : loop_(loop), http_events_(this)
    {
    }

    Server::~Server()
    {
        close();
    }

    void Server::addListen(int port, const std::string &bind_ip) {
        std::shared_ptr<uvw::TCPHandle> handle = loop_->resource<uvw::TCPHandle>();

        handle->on<uvw::ListenEvent>([this](const uvw::ListenEvent &, uvw::TCPHandle &srv) {
          std::shared_ptr<uvw::TCPHandle> client = srv.loop().resource<uvw::TCPHandle>();
          srv.accept(*client);
          acceptClient(client);
        });

        handle->bind(bind_ip, port);
        handle->listen();

        server_handles_.insert(handle);
    }

    void Server::close() {
        for(auto iter = server_handles_.begin(); iter != server_handles_.end(); ) {
            (*iter)->close();
            iter = server_handles_.erase(iter);
        }
    }

    void Server::acceptClient(std::shared_ptr<uvw::TCPHandle> client) {
        std::shared_ptr<Client> instance(new Client());

        client->data(instance);

        client->on<uvw::CloseEvent>([](const uvw::CloseEvent &evt, uvw::TCPHandle &client) {

        });
        client->on<uvw::EndEvent>([](const uvw::EndEvent &evt, uvw::TCPHandle &client) {
          client.close();
        });

        client->on<uvw::DataEvent>([this](const uvw::DataEvent &evt, uvw::TCPHandle &handle) {
          Client *client = reinterpret_cast<Client*>(handle.data().get());

          if (evt.length >= 0) {
              int parsed = client->execEvent(&http_events_, evt.data.get(), evt.length);
              if (parsed < evt.length) {
                  handle.close();
              }
          } else {
              handle.close();
          }
        });

        instance->start(instance, client);
        client->read();
    }

    void Server::httpRequestHandle(HttpRequest &req, HttpResponse &res) {
        res.setStatus(200);
        res.setHeader("X-Test", "OK");
        res.setNoBuffering(true);
        res << "NEW REQUEST" << std::endl;
        res << "URL = " << req.url << std::endl;
        res << "METHOD = " << req.method << std::endl;
    }

} // namespace jshttpserver
