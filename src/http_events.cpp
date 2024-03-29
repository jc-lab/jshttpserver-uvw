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

#include <http_parser.h>

namespace jshttpserver {

	class HttpEvents::Impl : public HttpEvents {
	private:
		http_parser_settings settings_;

		Handler* handler_;

	public:
		Impl(Handler* handler) : handler_(handler) {
			//
			// called once a connection has been made and the message is complete.
			//
			static std::function<int(http_parser * parser)> on_message_complete =
				[&](http_parser* parser) -> int {
				auto* client = reinterpret_cast<Client*>(parser->data);

				HttpRequest req;
				HttpResponse res;

				std::string method = client->getRawMethod();

				req.full_url = client->getUrl();
				req.raw_method = method;

				for (auto it = method.begin(); it != method.end(); it++) {
					*it = toupper(*it);
				}
				if (method == "GET") {
					req.method = METHOD_GET;
				}
				else if (method == "POST") {
					req.method = METHOD_POST;
				}
				else if (method == "PUT") {
					req.method = METHOD_PUT;
				}
				else if (method == "DELETE") {
					req.method = METHOD_DELETE;
				}
				else if (method == "OPTIONS") {
					req.method = METHOD_OPTIONS;
				}
				else if (method == "POST") {
					req.method = METHOD_POST;
				}
				else {
					req.method = METHOD_UNKNOWN;
				}

				res.setWrite([&](std::string str) {
					auto* client = reinterpret_cast<Client*>(parser->data);

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
				[](http_parser* parser, const char* at, size_t len) -> int {

				auto* client = reinterpret_cast<Client*>(parser->data);

				if (at && client) {
					client->setUrl(std::string(at, len));
				}
				return 0;
			};

			//
			// called when there are either fields or values in the request.
			//
			settings_.on_header_field =
				[](http_parser* parser, const char* at, size_t length) -> int {
				return 0;
			};

			settings_.on_header_value =
				[](http_parser* parser, const char* at, size_t length) -> int {
				return 0;
			};

			//
			// called once all fields and values have been parsed.
			//
			settings_.on_headers_complete =
				[](http_parser* parser) -> int {

				auto* client = reinterpret_cast<Client*>(parser->data);
				client->setRawMethod(http_method_str((enum http_method) parser->method));
				return 0;
			};

			//
			// called when there is a body for the request.
			//
			settings_.on_body =
				[](http_parser* parser, const char* at, size_t len) -> int {

				auto* client = reinterpret_cast<Client*>(parser->data);

				if (at && client) {
					client->setBody(std::string(at, len));
				}
				return 0;
			};

			//
			// called after all other events.
			//
			settings_.on_message_complete =
				[](http_parser* parser) -> int {
				return on_message_complete(parser);
			};
		}

		int exec(void* parser, const char* data, size_t len) override {
			http_parser* real_parser = reinterpret_cast<http_parser*>(parser);
			return (int)http_parser_execute(real_parser, &settings_, data, len);
		}
	};

	std::unique_ptr<HttpEvents> HttpEvents::create(Handler *handler) {
		std::unique_ptr<HttpEvents> instance(new Impl(handler));
		return std::move(instance);
	}

} // namespace jshttpserver
