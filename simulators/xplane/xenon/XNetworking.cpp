// *********************************************************************************************************************
// *                                                Main clas for networking                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 mar 2019 at 19:59 *
// *********************************************************************************************************************

// My own includes
#include "XNetworking.h"
#include "xplane.hpp"
#include "settings.h"

using namespace xenon;
using namespace std;

// Указатель на единственный экземпляр объекта, работающего с сетью.

XNetworking * __worker = nullptr;

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                             Статический "конструктор"                                            *
// *                                                                                                                  *
// ********************************************************************************************************************

XNetworking * XNetworking::create() {

    if ( ! __worker ) __worker = new XNetworking();
    return __worker;

}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                        Поток "серверной" приемной сокеты.                                        *
// *                                                                                                                  *
// ********************************************************************************************************************

void * __receiving_thread( void * arg ) {

    XPlane::log("Receiver thread started successfully.");

    for (;;) {
        usleep(2);
        if ( __worker ) __worker->receive();
    }

    return nullptr;
};


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    The constructor                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

XNetworking::XNetworking() {

    __transmitter = -1;
    __receiver = -1;

    // ------------------------------------------------------------------------
    //                     Creating a receiving part.
    // ------------------------------------------------------------------------

    __init_receiver();

    // ------------------------------------------------------------------------
    //                     Creating a transmitting socket
    // ------------------------------------------------------------------------

    __transmitter = socket(AF_INET, SOCK_DGRAM, 0);
    if ( __transmitter < 0 ) {
        // Errors when creating the socket descriptor itself, there is no point in going on.
        sprintf( __out, "Creating socket error %d. Error code=%d, message=%s",
                  __transmitter, errno, strerror( errno )
        );
        XPlane::log( string( __out ) );
        return;
    };

    // Initialization of the address structure for standalone multicast socket.

    memset( & __transmitter_address, 0, sizeof( __transmitter_address ));
    __transmitter_address.sin_family=AF_INET;
    __transmitter_address.sin_addr.s_addr=inet_addr( NETWORK_MULTICAST_GROUP );
    __transmitter_address.sin_port=htons( NETWORK_PORT );

#ifdef DEBUG
    sprintf( __out, "Network transmitting socket is open now. Port=%d, multicast group=%s", NETWORK_PORT, NETWORK_MULTICAST_GROUP );
    XPlane::log( __out );
#endif

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Инициализация приемной части                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void XNetworking::__init_receiver() {

    __receiver = socket(AF_INET, SOCK_DGRAM, 0);
    if ( __receiver < 0) {
        sprintf(__out, "Creating receiver socket error %d, code=%d, message=%s",
                  __receiver, errno, strerror( errno )
        );
        XPlane::log( __out );
        return;
    }

    int yes = 0;

//     if ( setsockopt( __receiver, IPPROTO_IP, IP_MULTICAST_LOOP, &yes, sizeof(yes) ) < 0 ) {
//         sprintf( __out, "Setting IP_MULTICAST_LOOP, error=%d, msg=%s", errno, strerror( errno ) );
//         XPlane::log( __out );
//         __close_socket( __receiver );
//         return;
//     };
//

    // Множество сокет могут использовать тот же самый порт, потому что
    // на мультикастинге "сервер" на одной и той же машине может быть -
    // не один.

    yes = 1;

    if ( setsockopt( __receiver, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        XPlane::log("Can not set REUSE_ADDR, exit.");
        __close_socket( __receiver );
        return;
    }

    yes = 1;

    if (setsockopt( __receiver, SOL_SOCKET, SO_REUSEPORT, (const char*) & yes, sizeof( yes )) < 0) {
        sprintf(__out, "ERROR while try to set SO_REUSEPORT %d, msg=%s", errno, strerror( errno ) );
        XPlane::log( __out );
        __close_socket( __receiver );
        return;
    }

    // Заполнение адресной структуры принимающей сокеты. Адреса здесь могут быть "любые",
    // мы же типа о "сервере" говорим.

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr=htonl(INADDR_ANY);
    sin.sin_port=htons( NETWORK_PORT );

    // Связывание приемной ("серверной") сокеты.

    if ( bind( __receiver, ( struct sockaddr * ) & sin, sizeof( sin ) ) < 0 ) {
        XPlane::log("ERROR when binding receiver socket.");
        __close_socket( __receiver );
        return;
    }

    // Установка на приемную ("серверную") сокету - мультикастинга.

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr=inet_addr( NETWORK_MULTICAST_GROUP );
    mreq.imr_interface.s_addr=htonl( INADDR_ANY );
    if ( setsockopt ( __receiver, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        XPlane::log("ERROR while set multicast group for receiving socket.");
        __close_socket( __receiver );
        return;
    }

    // Все. Можно запускать серверный поток слушания. Запуск потока приемника.

    int res = pthread_create( & __receiver__thread_id, NULL, __receiving_thread, NULL );
    if ( res < 0 ) {
        sprintf( __out, "Receiver thread start error %d. Code=%d, message=%s", res, errno, strerror( errno ) );
        XPlane::log( __out );
        __close_socket( __receiver );
        return;
    };

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Один "тик" принимающего потока                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void XNetworking::receive() {

    // Если по каким-то причинам принимающая сокета
    // имеет ошибочный дескриптор - выходим и больше сюда не возвращаемся.
    if ( __receiver < 0 ) {
        int result = 0;
        XPlane::log("Incorrect receiver socket, exit from thread.");
        pthread_exit( & result );
    };

        // Структуру объявляем в цикле. Так получаем гарантированную очистку стркутуры и ее независимость.
    struct sockaddr_in client_socket;
    memset( & client_socket, 0, sizeof( client_socket ) );
    socklen_t client_socket_len = sizeof( client_socket );

    memset(__rx_buffer, 0, sizeof( __rx_buffer ) );

    int received = recvfrom( __receiver, ( char * ) & __rx_buffer , sizeof( __rx_buffer ), 0,
        ( struct sockaddr * ) & client_socket, & client_socket_len
    );

    if ( received > 0 ) {
        try {
            JSON json = JSON::parse( (char *) __rx_buffer );
            string command_name = json.value("command_name", "UNKNOWN");

            // Никакого else здесь не надо. Реагируем - только на те команды,
            // приход которых от сети мы ждем.

            if ( command_name == CommandSet::COMMAND_NAME ) {
                CommandSet cmd( CommandSet::SET_UNKNOWN, 0, 0.0);
                cmd.from_json( json );
                if ( __setter ) __setter->set( cmd );
            }

        } catch ( JSON::exception & e ) {

            sprintf( __out, "XNetworking::receive() %s", e.what() );
            XPlane::log( __out );

        } catch ( std::exception & e ) {

            sprintf( __out, "XNetworking::receive() std exception: %s", e.what() );
            XPlane::log( __out );

        } catch ( ... ) {

            XPlane::log( "XNetworking::receive(), unhandled exception" );

        }
    };

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *             Close the socket and set descriptor to negative value, to avoid future use of the socket.             *
// *                                                                                                                   *
// *********************************************************************************************************************

void XNetworking::__close_socket( int & socket ) {

    ::close( socket );
    socket = -1;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                     Setting flags for re-use port and re-use IP address for specified socket.                     *
// *                                                                                                                   *
// *********************************************************************************************************************

/*
bool XNetworking::__prepare_socket_for_reuse( int & socket ) {

    char yes = 0;

//    // Disable loopback receiving ourself packet.
//
//    if ( setsockopt( socket, IPPROTO_IP, IP_MULTICAST_LOOP, &yes, sizeof(yes) ) < 0 ) {
//        sprintf( __out, "Setting IP_MULTICAST_LOOP, error=%d, msg=%s", errno, strerror( errno ) );
//        XPlane::log( __out );
//        return false;
//    };

    yes = 1;

    if ( setsockopt( socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0 ) {
        sprintf( __out, "Try to set SO_REUSEADDR, error=%d, msg=%s.", errno, strerror( errno ) );
        XPlane::log( __out );
        return false;
    }

#if IBM
#else

    yes = 1;
    if (setsockopt( socket, SOL_SOCKET, SO_REUSEPORT, (const char*) & yes, sizeof( yes )) < 0 ) {
        sprintf( __out, "Try to set SO_REUSEPORT, error=%d, msg=%s.", errno, strerror( errno ) );
        XPlane::log( __out );
        return false;
    }

#endif

    return true;
}
*/

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Send a broadcast message "to all listeners" using mulicast socket                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void XNetworking::send_to_all( void * cmd, int len ) {

    if ( __transmitter < 0 ) return;

    int result = sendto( __transmitter,
        ( char * ) cmd, len, 0,
        ( struct sockaddr * ) & __transmitter_address,
        sizeof( __transmitter_address )
    );

    if ( result != len ) {

#ifdef DEBUG

        sprintf( __out, "XNetworking::sent_to_all, error packet sending, res=%d, want=%d. Error=%d. msg=%s.",
            result, len, errno, strerror( errno )
        );
        XPlane::log( __out );

#endif

    };

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Send a command to all listeners using multicast socket                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void XNetworking::send_to_all(xenon::JSONAble * command) {

    try {

        JSON json;
        command->to_json( json );
        std::string str = json.dump();

        // Посмотреть, что там передается.

        // strcpy( __out, str.c_str() );
        // XPlane::log( __out );

        send_to_all( (void * )str.c_str(), (int) str.size() );

    } catch ( JSON::exception & e ) {

#ifdef DEBUG
        XPlane::log( string("XNetworking::send_to_all(), JSON exception: ") + e.what() );
#endif

    } catch (std::exception & e) {

#ifdef DEBUG
        XPlane::log( string("XNetworking::send_to_all(), standard exception: ") + e.what() );
#endif

    }

}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   The destructor                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

XNetworking::~XNetworking() {

    pthread_cancel( __receiver__thread_id );
    XPlane::log("Wait while receiving pthread finished...");
    pthread_join( __receiver__thread_id, nullptr );
    XPlane::log("Receiving pthread stopped.");

    // Now closing both sockets.
    if ( __transmitter >= 0 ) __close_socket( __transmitter );
    if ( __receiver >= 0 ) __close_socket( __receiver );


}
