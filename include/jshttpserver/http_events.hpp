/**
 * @file	http_events.hpp
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/ )
 * @date    2019/09/17
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JSHTTPSERVER_HTTP_EVENTS_HPP__
#define __JSHTTPSERVER_HTTP_EVENTS_HPP__

namespace jshttpserver {

    struct HttpRequest;
    class HttpResponse;

    class HttpEvents {
    public:
        class Handler {
        public:
            virtual void httpRequestHandle(HttpRequest &req, HttpResponse &res) = 0;
        };

    private:
		class Impl;

    public:
		static std::unique_ptr<HttpEvents> create(Handler* handler);
		virtual int exec(void* parser, const char* data, size_t len) = 0;

    };

} // namespace jshttpserver

#endif //__SRC_HTTP_EVENTS_HPP__
