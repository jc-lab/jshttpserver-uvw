/**
 * @file	client.cpp
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/ )
 * @date    2019/09/17
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <jshttpserver/client.hpp>

#include <http_parser.h>

namespace jshttpserver {

	class Client::Impl : public Client {
	private:
		http_parser parser_;
	
	public:
		void start(std::shared_ptr<Client> self, std::shared_ptr<uvw::TCPHandle> handle) override {
			handle_ = handle;
			http_parser_init(&parser_, HTTP_REQUEST);
			parser_.data = this;
		}

		int execEvent(HttpEvents* http_events, const char* data, size_t len) override {
			return http_events->exec(&parser_, data, len);
		}
	};

    Client::Client() {
    }

    Client::~Client() {
    }

	std::shared_ptr<Client> Client::create() {
		std::shared_ptr<Client> instance(new Impl());
		return instance;
	}

} // namespace jshttpserver
