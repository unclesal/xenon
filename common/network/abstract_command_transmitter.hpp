// *********************************************************************************************************************
// *                                Преобразование и передача в сеть одной абстрактной команды                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 14:22 *
// *********************************************************************************************************************
#pragma once

#include <vector>
#include "abstract_command.h"
#include "settings.h"

namespace xenon {
    class AbstractCommandTransmitter {
        
        public:
            
            AbstractCommandTransmitter( int socket ) {
                __socket = socket;
            };
            
            bool transmitt( AbstractCommand * cmd, std::string & error ) {
                if ( ! cmd ) {
                    error = "AbstractCommandTransmitter::transmitt() called with empty command";
                    return false;
                };
                
                memset( __tx_buffer, 0, sizeof( __tx_buffer ));
                error = "";
                JSON json;
                cmd->to_json( json );                
                std::vector<std::uint8_t> trx = JSON::to_ubjson(json);
            
                if ( trx.size() >= (sizeof( __tx_buffer ) - 2 )) {
                    // Не войдет в чаровый буфер.                    
                    error = "command " + cmd->command_name() + " has length " + std::to_string(trx.size()) 
                        + ", can not be transmitted.";
                    return false;
                
                } else {
                
                    // Нам нужен char *
                    std::copy( trx.begin(), trx.end(), __tx_buffer );
                
                    // Длина передаваемого UBJSONа.
                    uint16_t len = trx.size();
                    ssize_t sent = ::write( __socket, (char * ) & len, sizeof(len));
                    if ( sent != sizeof(len)) {
                        error = "Can not send lenght of packet";
                        return false;
                    }
                    
                    sent = ::write( __socket, __tx_buffer, trx.size() );
                    if ( sent != trx.size() ) {
                        error = "want " + std::to_string( trx.size() )
                        + ", but sent " + std::to_string( sent );
                        return false;            
                    }
                    
                    return true;

                };

            };
            
        private:
            int __socket;
            char __tx_buffer[ COMMUNICATOR_MAX_PACKET_SIZE ];
        
    }; // AbstractCommandTransmitter
}; // namespace xenon
