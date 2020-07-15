// *********************************************************************************************************************
// *                  Клиентская часть общения с коммуникатором, будем считать, что уже присоединенная                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 05:55 *
// *********************************************************************************************************************

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include <thread>
#include <string>
#include <iostream>
#include <deque>
#include <mutex>

#include "settings.h"
#include "structures.h"
#include "command_parser.h"
#include "connected_communicator_reactor.h"

namespace xenon {
    
    class ConnectedCommunicator {
    
        public:

            ConnectedCommunicator( 
                ConnectedCommunicatorReactor * reactor
            );
            ~ConnectedCommunicator();
            
            bool is_connected() {
                return __connected;
            };
            
            void disconnect() {
                __close_socket();
            };
            
            /**
             * @short Передать пакет в сеть.
             */
            void transmitt( AbstractCommand & cmd );                        
            
            void network_step();
            
            void init_communicator() {
                __inited = true;                
            };
            
            bool communicator_inited() {
                return __inited;
            };
            
        protected:
            
        private:
            
            /**
             * @short Нумерация отправленных пакетов в порядке возрастания.
             * Просто для контроля, на что конкретно мы получили ответ.
             */
            static uint16_t __packet_number;
            int __socket;            
            char __rx_buffer[ COMMUNICATOR_MAX_PACKET_SIZE ];
            char __transmitt_buffer[ COMMUNICATOR_MAX_PACKET_SIZE ];
                        
            ConnectedCommunicatorReactor * __reactor;                        
            bool __inited;
                                    
            CommandParser __parser;
            bool __connected;
                                    
            void __read_from_socket();
            
            void __try_open_socket();
            void __close_socket();            
        
    }; // CommunicatorClient
    
}; // xenon
