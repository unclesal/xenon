// *********************************************************************************************************************
// *                                Преобразование и передача в сеть одной абстрактной команды                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 14:22 *
// *********************************************************************************************************************
#pragma once

#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "abstract_command.h"
#include "settings.h"

namespace xenon {
    
    class AbstractCommandTransmitter {
        
        public:
            
            AbstractCommandTransmitter( int socket ) {
                __socket = socket;
            };
            
            bool transmitt( AbstractCommand * cmd, std::string & error ) {
                
                if ( __socket < 0 ) return false;
                
                char transmitt_buffer[ COMMUNICATOR_MAX_PACKET_SIZE ];
                
                cout << "On transmitt, socket=" << __socket << endl;
                
                
                if ( ! cmd ) {
                    error = "AbstractCommandTransmitter::transmitt() called with empty command";
                    return false;
                };
                
                cout << "Before memset..." << endl;
                
                memset( transmitt_buffer, 0, sizeof( transmitt_buffer ));
                
                cout << "memset done." << endl;
                error = "";
                JSON json;
                cmd->to_json( json );                
                std::vector<std::uint8_t> trx = JSON::to_ubjson(json);
                
                cout << "Jsonified." << endl;
            
                if ( trx.size() >= (sizeof( transmitt_buffer ) - 2 )) {
                    // Не войдет в чаровый буфер.                    
                    error = "command " + cmd->command_name() + " has length " + std::to_string(trx.size()) 
                        + ", can not be transmitted.";
                    return false;
                
                } else {
                                    
                    uint16_t len = trx.size();
                    ssize_t total = len + sizeof( len );
                    
                    // Длина передаваемого UBJSONа.                    
                    memcpy( & transmitt_buffer[0], &len, sizeof( len ) );
                    
                    // Нам нужен char *
                    // std::copy( trx.begin(), trx.end(), &__transmitt_buffer[ sizeof( len ) ] );                                        
                    memcpy( & transmitt_buffer[ sizeof( len ) ], trx.data(), trx.size() );                                        
                    
                    cout << "Len = " << len << ", total=" << total << ", socket=" << __socket << ", before write." << endl;
                    
                    ssize_t sent = ::write( __socket, transmitt_buffer, total );
                    
                    cout << "Wrote " << sent << endl;
                    
                    if ( sent != total ) {
                        error = "AbstractCommandTransmitter::transmitt(), want " + std::to_string( trx.size() )
                        + ", but sent " + std::to_string( sent ) + ", error=" + strerror( errno ) ;
                        return false;            
                    }
                    
                    cout << "Transmitter::transmitt: " << cmd->command_name() << ", sent=" << sent << ", want " << trx.size() + sizeof( len ) << endl;
                    
                    return true;

                };
                
                return false;

            };
            
        private:
            int __socket;            
        
    }; // AbstractCommandTransmitter
}; // namespace xenon
