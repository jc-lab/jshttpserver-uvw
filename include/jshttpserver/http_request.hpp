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

    enum Method {
        METHOD_UNKNOWN = 0,
        METHOD_GET = 0x0001,
        METHOD_POST = 0x0002,
        METHOD_PUT = 0x0004,
        METHOD_DELETE = 0x0008,
        METHOD_OPTIONS = 0x0010,
        METHOD_ALL = -1
    };

    struct HttpRequest {
        std::string url;
        Method method;
        std::string raw_method;
        std::string status_code;
        std::string body;
        std::map<const std::string, const std::string> headers;
        std::map<const std::string, const std::string> path_variables;
    };

} // namespace jshttpserver

#endif //__JSHTTPSERVER_HTTP_REQUEST_HPP__
