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

#include <set>

#include <uvw.hpp>

#include "http_events.hpp"

namespace jshttpserver {
class Server : private HttpEvents::Handler {
    private:
        std::shared_ptr<uvw::Loop> loop_;
        std::set<std::shared_ptr<uvw::TCPHandle>> server_handles_;

        HttpEvents http_events_;

        Server(std::shared_ptr<uvw::Loop> loop);

        void acceptClient(std::shared_ptr<uvw::TCPHandle> client);

    public:
        virtual ~Server();

        static std::shared_ptr<Server> create(std::shared_ptr<uvw::Loop> loop);
        void addListen(int port, const std::string& bind_ip = "0.0.0.0");
        void close();

    protected:
        void httpRequestHandle(HttpRequest &req, HttpResponse &res) override;
    };
}

#endif //__JSHTTPSERVER_SERVER_HPP__
