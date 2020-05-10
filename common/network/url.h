// *********************************************************************************************************************
// *                                                        URL parser                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 mar 2020 at 19:11 *
// *********************************************************************************************************************

#pragma once
#include <string>

namespace xenon {
    class URL {
        public:
            URL(const std::string & url_s);
            ~URL() = default;
            std::string protocol() {
                return _protocol;
            };
            
            std::string host() {
                return _host;
            };
            
            std::string path() {
                return _path;
            };
            
            std::string query() {
                return _query;
            };
            
        private:
            void parse(const std::string & url_s);
            std::string _protocol, _host, _path, _query;
    };
}; // namespace xenon
