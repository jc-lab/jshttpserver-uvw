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

#include <http_parser.h>

namespace jshttpserver {

    class HttpRequest;
    class HttpResponse;

    class HttpEvents {
    public:
        class Handler {
        public:
            virtual void httpRequestHandle(HttpRequest &req, HttpResponse &res) = 0;
        };

    private:
        http_parser_settings settings_;
        Handler *handler_;

    public:
        HttpEvents(Handler *handler);

        int exec(http_parser *parser, const char *data, size_t len) {
            return (int) http_parser_execute(parser, &settings_, data, len);
        }

    };

} // namespace jshttpserver

#endif //__SRC_HTTP_EVENTS_HPP__
