// *********************************************************************************************************************
// *                                          Периодически обновляемый файл                                            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 mar 2020 at 12:06 *
// *********************************************************************************************************************

#ifdef IVAO

#include <iostream>
#include <ctime>
#include <fstream>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
// #include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>

#include "ivao_periodically_updated_file.h"
#include "xplane_utilities.h"
#include "base64.h"

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор.                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

xenon::IVAOPeriodicallyUpdatedFile::IVAOPeriodicallyUpdatedFile() {
    _refreshable_interval_in_seconds = 0;
    _file_name = std::string("empty.txt");
    
    _host = std::string("localhost");
    _scheme = std::string("http");
    _url = std::string("/");
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *            Выдать собственный полный путь. В данном случае - пустая функция, ее надо перегружать.                 *
// *                                                                                                                   *
// *********************************************************************************************************************

std::string xenon::IVAOPeriodicallyUpdatedFile::path() {
    // На пока что все обновляемые файлики лежат в 
    // том же самом директории, что и сам плагин.
    std::string the_path = XPlaneUtilities::get_plugin_path() + _file_name;
    return the_path;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Можно ли уже перезагрузить этот файл?                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

bool xenon::IVAOPeriodicallyUpdatedFile::can_be_updated() {
    // Полный путь.
    std::string the_path = path();
    
    bool exists = boost::filesystem::exists(the_path);

//    std::string message = std::string("File name=") + the_path + ", exists? " + std::to_string(exists);
//    XPlaneUtilities::log(message);

    if (exists) {
        std::time_t modification_time = boost::filesystem::last_write_time(the_path);
        int refreshable_interval = get_refresh_interval();
        if ( ! refreshable_interval ) {
            // Интервал обновления файла - либо не перекрыт потомками,
            // либо мы специально не хотели обновлять этот файл.
            return false;
        };

        modification_time += get_refresh_interval();
        // Указание std - чтобы точно брались функции из пространства имен
        // std, т.к. точно такие же функции есть чисто C-шные.
        std::time_t now = 0;
        std::time( & now );

//        message = "modification=" + std::to_string(modification_time)
//                + ", now=" + std::to_string(now) + ", true? "
//                + std::to_string(now < modification_time);
//        XPlaneUtilities::log(message);

        if ( now < modification_time ) return false;
    }

    // Файлика - либо нет, либо его интервал обновления
    // уже вышел. В-общем, да, его можно обновлять.

    return true;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Получение содержимого с WEB                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

std::string xenon::IVAOPeriodicallyUpdatedFile::request_from_web() {

    int version = 10; // 10 || 11;

    // The io_context is required for all I/O
    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    // Look up the domain name
    tcp::resolver::query query(host(), scheme());
    auto const results = resolver.resolve(query);

    // Make the connection on the IP address we get from a lookup
    stream.connect(results);

    // Set up an HTTP GET request message
    http::request<http::string_body> req{http::verb::get, url(), version};
    req.set(http::field::host, host());
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    http::write(stream, req);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream, buffer, res);

    // Write the message to standard out
    // std::cout << res << std::endl;

    // Gracefully close the socket
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    // not_connected happens sometimes
    // so don't bother reporting it.
    //
    if (ec && ec != beast::errc::not_connected)
        throw beast::system_error{ec};

    // If we get here then the connection is closed gracefully
    return boost::beast::buffers_to_string(res.body().data());
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Перезагрузить файл с вэба и записать его на диск.                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

bool xenon::IVAOPeriodicallyUpdatedFile::reload() {
    
    try {
        
        std::string content = request_from_web();
        if ( content.length() ) {
            // Если что-то смысловое (не-нулевое) получили - записываем в файл статуса.
            ofstream ofs( path(), ofstream::out );
            ofs << content;
            ofs.close();
            return true;
        };
        
    } catch (std::exception const & e ) {
        std::string message = std::string("ERROR: IVAOPeriodicallyUpdatedFile::reload(): ") + e.what();
        XPlaneUtilities::log(message);
    }
    return false;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 GZIP-компрессия                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

std::string xenon::IVAOPeriodicallyUpdatedFile::gzip_encode(const std::string & data) {
    
    std::stringstream compressed;
    std::stringstream original;
    original << data;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(original);
    boost::iostreams::copy(out, compressed);

    /**need to encode here **/
    std::string compressed_encoded = base64_encode(
        reinterpret_cast<const unsigned char*>(compressed.str().c_str()), compressed.str().length()
    );

    return compressed_encoded;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               GZIP-декомпрессия.                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

std::string xenon::IVAOPeriodicallyUpdatedFile::gunzip_decode(const std::string & data) {

    /*
    std::stringstream compressed_encoded;
    std::stringstream decompressed;
    compressed_encoded << data;

    // first decode  then decompress
    std::stringstream compressed;
    compressed << base64_decode(compressed_encoded.str());

    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_decompressor());
    in.push(compressed);

    boost::iostreams::copy(in, decompressed);
    return decompressed.str();
    */

    namespace bio = boost::iostreams;

    std::stringstream compressed(data);
    std::stringstream decompressed;

    bio::filtering_streambuf<bio::input> out;
    out.push(bio::gzip_decompressor());
    out.push(compressed);
    bio::copy(out, decompressed);

    return decompressed.str();

}             
             
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Прочитать содержимое файла.                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void xenon::IVAOPeriodicallyUpdatedFile::read() {
    _lines.clear();
    std::ifstream file( path() );
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            _lines.push_back(line);
        }
        file.close();
    }
}

#endif // IVAO
