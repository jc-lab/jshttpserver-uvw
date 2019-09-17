/**
 * @file	http_response.hpp
 * @author	Jichan (development@jc-lab.net / http://ablog.jc-lab.net/ )
 * @date    2019/09/17
 * @copyright Copyright (C) 2019 jichan.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JSHTTPSERVER_HTTP_RESPONSE_HPP__
#define __JSHTTPSERVER_HTTP_RESPONSE_HPP__

#include <iostream>
#include <string>
#include <sstream>
#include <map>

#include <http_parser.h>

namespace jshttpserver {

    class HttpResponse : public std::ostream {
    private:
        typedef std::function<void (std::string s)> Callback;
        Callback write_;
        std::stringstream ss;

        bool no_buffering_;

        class Buffer : public std::stringbuf {
        public:
            HttpResponse *res;
            bool header_written_;

            std::stringstream buf_;

            Buffer(std::ostream &str) : header_written_(false) {}
            void done() {
                if(!res->no_buffering_) {
                    std::string s = buf_.str();
                    writeHeader(s.length());
                    res->write_(s);
                }else{
                    res->write_("0\r\n\r\n");
                }
            }

            int writeHeader(int64_t contentLength) {
                std::ostringstream ss;

                ss << "HTTP/1.1 " << res->status << " OK\r\n";

                for (auto &h: res->headers)
                    ss << h.first << ": " << h.second << "\r\n";

                //
                // set the content length and content.
                //
                if(contentLength >= 0) {
                    ss << "Content-Length: " << contentLength << "\r\n";
                }

                ss << "\r\n";

                res->write_(ss.str());

                return 0;
            }

            int sync() override {
                std::string s = str();
                str("");

                if(res->no_buffering_) {
                    if(!header_written_) {
                        res->headers.insert({"Transfer-Encoding", "chunked"});
                        writeHeader(-1);
                        header_written_ = true;
                    }
                    {
                        char buffer[64];
                        snprintf(buffer, sizeof(buffer), "%x\r\n", s.length());
                        res->write_(buffer);
                        res->write_(s + "\r\n");
                    }
                }else{
                    buf_ << s;
                }

                return 0;
            }
        };

        Buffer buffer;

    public:
        std::map<const std::string, const std::string> headers;
        int status = 200;

        void setWrite(Callback cb) {
            write_ = cb;
        }

        void setHeader(const std::string key, const std::string val) {
            headers.insert({ key, val });
        }

        void setStatus(int code) {
            status = code;
        }

        HttpResponse() :
            std::ostream(&buffer),
            buffer(ss),
            no_buffering_(false)
            {
            buffer.res = this;
        }
        ~HttpResponse() {
            buffer.done();
        }

        bool getNoBuffering() const {
            return no_buffering_;
        }

        void setNoBuffering(bool no_buffering) {
            no_buffering_ = no_buffering;
        }
    };

} // namespace jshttpserver

#endif //__JSHTTPSERVER_HTTP_RESPONSE_HPP__
