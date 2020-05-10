// *********************************************************************************************************************
// *                                                        URL parser                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 mar 2020 at 19:15 *
// *********************************************************************************************************************

#include "url.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
using namespace std;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                      Конструктор.                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

xenon::URL::URL(const std::string & url_s) {
    _protocol = _host = _query = _path = "";
    parse(url_s);
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                      The parser                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void xenon::URL::parse(const string & url_s) {
    const string prot_end("://");
    string::const_iterator prot_i = search(
        url_s.begin(), url_s.end(), prot_end.begin(), prot_end.end()
    );
    
    _protocol.reserve(distance(url_s.begin(), prot_i));
    transform(
        url_s.begin(), prot_i,
        back_inserter(_protocol),
        ptr_fun<int,int>(tolower)
    ); // protocol is icase

    if ( prot_i == url_s.end() )
        return;
    
    advance(prot_i, prot_end.length());
    
    string::const_iterator path_i = find(prot_i, url_s.end(), '/');
    _host.reserve(distance(prot_i, path_i));
    
    transform(
        prot_i, path_i,
        back_inserter(_host),
        ptr_fun<int,int>(tolower)
    ); // host is icase
    
    string::const_iterator query_i = find(path_i, url_s.end(), '?');
    _path.assign(path_i, query_i);
    if( query_i != url_s.end() )
        ++query_i;
    
    _query.assign(query_i, url_s.end());
}
