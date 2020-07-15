// *********************************************************************************************************************
// *                    Клиентская часть общения с коммуникатором, будем считать, что уже соединенная.                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 05:58 *
// *********************************************************************************************************************
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netdb.h>

#include "connected_communicator.h"
#include "utils.hpp"

using namespace xenon;

uint16_t ConnectedCommunicator::__packet_number = 0;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор.                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

ConnectedCommunicator::ConnectedCommunicator( 
    ConnectedCommunicatorReactor * reactor
) {
    
    __reactor = reactor;
    __socket = -1;    
    __connected = false;
    __inited = false;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Один "шаг" коммуникатора                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::network_step() {
    
    if ( ! __inited ) return;
    if ( ! __connected ) __try_open_socket();
    if ( __connected ) __read_from_socket();
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Попытка открытия сокеты                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::__try_open_socket() {
    
    if ( ! __inited ) return;
    
    if ( ( __socket < 0 ) || ( ! __connected ) ) {            
        
        // Попытка открытия сокеты. Причем, может быть такая ситуация, что сама сокета-то 
        // открыта, но вот соединиться мы не смогли. Учитываем эту ситуацию.        
        __connected = false;
        if ( __socket >= 0 ) __close_socket();
        
        __socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if ( __socket < 0 ) return;
        
        hostent * remote_communicator = gethostbyname( get_communicator_host().c_str() );
        if ( ! remote_communicator ) {
            __close_socket();      
            __reactor->on_error(
                "ConnectedCommunictor::__receiv(), no such host " + get_communicator_host()
            );
            return;
        }
        
        sockaddr_in server_addr;
        memset( &server_addr, 0, sizeof( server_addr ) );
        server_addr.sin_family = AF_INET;
        bcopy((char *) remote_communicator->h_addr, (char *) & server_addr.sin_addr.s_addr, remote_communicator->h_length);            
        server_addr.sin_port = htons( get_communicator_port() );
        
        if ( ::connect( __socket, (struct sockaddr * ) & server_addr, sizeof( server_addr)) < 0) {
            if ( errno != ECONNREFUSED ) // connection refused
            {
                __reactor->on_error(
                    "ConnectedCommunicator::connect: " + std::to_string( errno ) 
                    + ", message=" + std::string( strerror( errno ))
                );
            }
            __close_socket();
            return;
        }

        // Если здесь остались - мы соединены.
        // Делаем сокету не-блоковой.
        
        int flags = fcntl( __socket, F_GETFL, 0);
        flags |= O_NONBLOCK;
        fcntl( __socket, F_SETFL, flags);
        
        __connected = true;
        __reactor->on_connect();

    };
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *               Запрос от клиентской стороны коммуникатору. Точнее, это только складывание в очередь                *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::transmitt( AbstractCommand & cmd ) {            
        
    if ( ! communicator_inited() ) return;
    if ( ! is_connected() ) return;
    if ( __socket < 0 ) return;
    
    // Номера передаваемых команд по порядку.
    
    ConnectedCommunicator::__packet_number ++;
    if ( ConnectedCommunicator::__packet_number >= (uint16_t) 0xffffff0 ) ConnectedCommunicator::__packet_number = 0;
    cmd.__packet_number = ConnectedCommunicator::__packet_number;
    
    // Системное время - на момент складывания пакета в очередь.
    // Т.е. именно тогда, когда координаты были валидны. Задержка
    // на нахождение в очереди перед передачей будет учтена сама собой.
    
    cmd._vcl_condition.agent_system_time_ms = xenon::get_system_time_ms();
    
    JSON json;
    cmd.to_json( json );                
    std::vector<std::uint8_t> trx = JSON::to_ubjson(json);
        
    uint16_t len = trx.size();
    ssize_t total = len + sizeof( len );
    memcpy( __transmitt_buffer, & len, sizeof( len ) );
    memcpy( & __transmitt_buffer[ sizeof(len) ], trx.data(), trx.size() );
        
    ssize_t sent = ::write( __socket, __transmitt_buffer, total );
    
    if ( sent != total ) {
        __reactor->on_error(
            "ConnectedCommunicator::transmitt(), want " + std::to_string( total )
            + ", sent " + std::to_string( sent ) + ", error=" + strerror( errno )
        );
        __close_socket();
        return;
    };        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Чтение с сокеты (блокирует поток)                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::__read_from_socket() {
        
    for ( ;; ) {
        
        int total;
        ioctl( __socket, FIONREAD, & total );        
                
        uint16_t len = 0;
        if ( total < sizeof( len ) ) {
            return;
        }
        
        ssize_t bytes = recv( __socket, & len, sizeof( len ), MSG_PEEK );
        if ( bytes < 0 ) {
            __reactor->on_error(
                "ConnectedCommunicator::read length, " + to_string( bytes )
                + ", error=" + strerror( errno )
            );
            // Сетевая ошибка.
            disconnect();
            return;
        }
        
        int wanted = len + sizeof( len );
                
        if ( total >= wanted ) {
            
            // Там есть полная команда. Вычитываем ее полностью.
            bytes = read( __socket, & len, sizeof( len ) );            
            bytes = read( __socket, __rx_buffer, len );
                        
            if ( bytes != len ) {
                __reactor->on_error("Read from socket, want " + std::to_string( len ) + ", but read " + std::to_string( bytes) );
                disconnect();
                return;
            }
            
            // В буфере получили полностью пакет с командой.                        
            // Если сколько-то байт было принято - парзим.
            std::string command_name;
            AbstractCommand * cmd = __parser.parse( __rx_buffer, bytes, command_name );
    
            if ( ! cmd ) {
                __reactor->on_error(
                    "ConnectedCommunicator::__read_from_socket(), got " + std::to_string( bytes ) 
                    + ", but command was not recognized. Command name=" + command_name            
                );
                __close_socket();
                return;
            }
    
            __reactor->on_received( cmd );
            
            delete( cmd );
            cmd = nullptr;
        
        } else {
            // Еще не вся команда собралась в буфере приемника, ждем следующего "шага"
            return;
        }
        
    }    
                
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Закрытие сокеты                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::__close_socket() {
        
    if ( __socket >= 0 ) {
        ::close( __socket );
        __socket = -1;        
    }
    
    if ( __connected ) {
        // Чтобы вызывалось не в цикле, а только один раз - 
        // если раньше было соединение, а теперь его не стало.
        __reactor->on_disconnect();
    }
    
    __connected = false;
    
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Деструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

ConnectedCommunicator::~ConnectedCommunicator() {
    __close_socket();
}
