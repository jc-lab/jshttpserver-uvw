/**
 * @file	http_events.cpp
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/ )
 * @date    2019/09/17
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <functional>

#include <jshttpserver/client.hpp>
#include <jshttpserver/http_events.hpp>
#include <jshttpserver/http_request.hpp>
#include <jshttpserver/http_response.hpp>

namespace jshttpserver {

    HttpEvents::HttpEvents(Handler *handler) : handler_(handler) {
        //
        // called once a connection has been made and the message is complete.
        //
        static std::function<int(http_parser *parser)> on_message_complete =
            [&](http_parser *parser) -> int {
              auto *client = reinterpret_cast<Client*>(parser->data);

              HttpRequest req;
              HttpResponse res;

              req.url = client->getUrl();
              req.method = client->getMethod();

              res.setWrite([&](std::string str) {
                  auto *client = reinterpret_cast<Client*>(parser->data);

                  std::unique_ptr<char[]> write_buf(new char[str.length()]);
                  memcpy(write_buf.get(), str.c_str(), str.length());

                  client->handle()->write(std::move(write_buf), str.length());
              });

              handler_->httpRequestHandle(req, res);

              return 0;
            };

        memset(&settings_, 0, sizeof(settings_));

        //
        // called after the url has been parsed.
        //
        settings_.on_url =
            [](http_parser *parser, const char *at, size_t len) -> int {

              auto *client = reinterpret_cast<Client*>(parser->data);

              if (at && client) {
                  client->setUrl(std::string(at, len));
              }
              return 0;
            };

        //
        // called when there are either fields or values in the request.
        //
        settings_.on_header_field =
            [](http_parser *parser, const char *at, size_t length) -> int {
              return 0;
            };

        settings_.on_header_value =
            [](http_parser *parser, const char *at, size_t length) -> int {
              return 0;
            };

        //
        // called once all fields and values have been parsed.
        //
        settings_.on_headers_complete =
            [](http_parser *parser) -> int {

              auto *client = reinterpret_cast<Client*>(parser->data);
              client->setMethod(http_method_str((enum http_method) parser->method));
              return 0;
            };

        //
        // called when there is a body for the request.
        //
        settings_.on_body =
            [](http_parser *parser, const char *at, size_t len) -> int {

              auto *client = reinterpret_cast<Client*>(parser->data);

              if (at && client) {
                  client->setBody(std::string(at, len));
              }
              return 0;
            };

        //
        // called after all other events.
        //
        settings_.on_message_complete =
            [](http_parser *parser) -> int {
              return on_message_complete(parser);
            };
    }

} // namespace jshttpserver
