/**
 * @file	http_request.hpp
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/ )
 * @date    2019/09/17
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JSHTTPSERVER_HTTP_REQUEST_HPP__
#define __JSHTTPSERVER_HTTP_REQUEST_HPP__

#include <string>
#include <map>

#include <http_parser.h>

namespace jshttpserver {

    struct HttpRequest {
        std::string url;
        std::string method;
        std::string status_code;
        std::string body;
        std::map<const std::string, const std::string> headers;
    };

} // namespace jshttpserver

#endif //__JSHTTPSERVER_HTTP_REQUEST_HPP__
