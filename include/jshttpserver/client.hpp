/**
 * @file	client.hpp
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/ )
 * @date    2019/09/17
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JSHTTPSERVER_CLIENT_HPP__
#define __JSHTTPSERVER_CLIENT_HPP__

#include <memory>
#include <string>

#include <uvw.hpp>
#include <http_parser.h>
#include "http_events.hpp"
#include "http_request.hpp"

namespace jshttpserver {
    class Client : protected HttpRequest {
    private:
        std::weak_ptr<uvw::TCPHandle> handle_;

        http_parser parser_;

    public:
        Client();
        void start(std::shared_ptr<Client> self, std::shared_ptr<uvw::TCPHandle> handle);
        int execEvent(HttpEvents *http_events, const char *data, size_t len);
        virtual ~Client();

        std::shared_ptr<uvw::TCPHandle> handle() {
            return handle_.lock();
        }

        const std::string &getUrl() const {
            return this->full_url;
        }
        void setUrl(const std::string &url) {
            this->full_url = url;
        }
        const std::string &getRawMethod() const {
            return this->raw_method;
        }
        void setRawMethod(const std::string &raw_method) {
            this->raw_method = raw_method;
        }
        const Method getMethod() const {
            return this->method;
        }
        const std::string &getBody() const {
            return this->body;
        }
        void setBody(const std::string &body) {
            this->body = body;
        }
    };
}

#endif //__JSHTTPSERVER_CLIENT_HPP__
