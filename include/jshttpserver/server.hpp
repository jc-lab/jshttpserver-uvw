/**
 * @file	server.hpp
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/ )
 * @date    2019/09/17
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JSHTTPSERVER_SERVER_HPP__
#define __JSHTTPSERVER_SERVER_HPP__

#include <memory>
#include <string>
#include <functional>

#include <set>
#include <map>
#include <regex>

#include <uvw.hpp>

#include "http_events.hpp"

#include <iostream>
namespace jshttpserver {
    class Server : private HttpEvents::Handler {
    public:
        typedef std::function<void(HttpRequest& req, HttpResponse &res)> RequestHandler;

    private:
        struct RequestMappingItem {
            bool doc_root_mapping;

            int methods;
            std::regex path_regex;
            RequestHandler handler;
            std::list<std::string> path_variables;

            RequestMappingItem(bool _doc_root_mapping, int _methods, const std::string &_path_regex, const RequestHandler &_handler, const std::list<std::string> &_path_variables) : doc_root_mapping(_doc_root_mapping), methods(_methods), path_regex(_path_regex), handler(_handler), path_variables(_path_variables) {
            }
        };

        std::shared_ptr<uvw::Loop> loop_;
        std::set<std::shared_ptr<uvw::TCPHandle>> server_handles_;

        HttpEvents http_events_;

        int64_t request_mappings_count_;
        std::map<int64_t, std::unique_ptr<RequestMappingItem>> request_mappings_;

        Server(std::shared_ptr<uvw::Loop> loop);

        void acceptClient(std::shared_ptr<uvw::TCPHandle> client);

        int64_t requestMappingImpl(bool doc_root_map, int methods, const std::string& path_regex, const RequestHandler &handler);

    public:
        virtual ~Server();

        static std::shared_ptr<Server> create(std::shared_ptr<uvw::Loop> loop);
        void addListen(int port, const std::string& bind_ip = "0.0.0.0");
        void close();

        int64_t requestMapping(int methods, const std::string& path_regex, const RequestHandler &handler);
        int64_t documentRootMapping(const std::string& path_prefix_regex, const RequestHandler &handler);
        void removeMapping(int64_t index);

    protected:
        void httpRequestHandle(HttpRequest &req, HttpResponse &res) override;
    };
}

#endif //__JSHTTPSERVER_SERVER_HPP__
