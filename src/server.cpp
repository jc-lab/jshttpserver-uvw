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

    int64_t Server::requestMapping(int methods, const std::string& path_matcher, RequestHandler handler) {
        int64_t index = ++request_mappings_count_;

        static std::regex pv_regex("\\{([a-zA-Z0-9_]+)(:([^}]+))?\\}");
        std::string path_regex(path_matcher);

        std::list<std::string> path_variables;

        std::smatch m;
        do {
            std::regex_search(path_regex, m, pv_regex);
            if (m.size() > 0) {
                std::string name = m.str(1);
                std::string subregex = m.str(3);

                if(!subregex.empty()) {
                    path_regex.replace(m.position(0), m.str(0).length(), "(" + subregex + ")");
                }else{
                    path_regex.replace(m.position(0), m.str(0).length(), "([^/]+)");
                }

                path_variables.push_back(name);
            }
        } while (m.size() > 0);

        std::unique_ptr<RequestMappingItem> item(new RequestMappingItem(methods, "^" + path_regex + "$", handler, path_variables));
        request_mappings_.insert( { index, std::move(item) } );

        return index;
    }

    void Server::removeRequestMapping(int64_t index) {
        auto iter = request_mappings_.find(index);
        if(iter != request_mappings_.end()) {
            request_mappings_.erase(iter);
        }
    }

    void Server::httpRequestHandle(HttpRequest &req, HttpResponse &res) {
        int path_result = 0;

        for(auto iter = request_mappings_.cbegin(); iter != request_mappings_.cend(); iter++) {
            RequestMappingItem *item = iter->second.get();
            std::sregex_iterator path_iter(req.url.cbegin(), req.url.cend(), item->path_regex);
            std::sregex_iterator path_end;

            if(path_iter != path_end) {
                if((item->methods < 0) || (item->methods & req.method)) {
                    const std::smatch &m = *path_iter;
                    auto it_pv_name = item->path_variables.cbegin();
                    int index = 1;
                    for (; index < m.size() && it_pv_name != item->path_variables.cend(); index++, it_pv_name++) {
                        req.path_variables.insert({*it_pv_name, m.str(index)});
                    }
                    item->handler(req, res);
                    path_result = 1;
                }else{
                    path_result = 2;
                }
                break;
            }
        }

        if(path_result == 2) {
            res.setStatus(405);
        }else if(path_result == 0){
            res.setStatus(404);
        }
    }

} // namespace jshttpserver
