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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>

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
            
            /**
             * @short Запрос от некоего агента к центральному коммуникатору.
             * Точнее, здесь идет только постановка в очередь. Фактическая отправка пакета на сервер
             * произойдет тогда, когда будет установлено соединение и очередь на отправку дойдет до данного
             * конкретного пакета. Некий аналог "ассинхронности", чтобы клиент не сидел и не ждал ответа.
             */
            void request( AbstractCommand * cmd );
            
        protected:
            
        private:
            
            /**
             * @short Нумерация отправленных пакетов в порядке возрастания.
             * Просто для контроля, на что конкретно мы получили ответ.
             * max = 4 294 967 295 (0xffffffff)
             */
            static unsigned int __packet_number;
                        
            ConnectedCommunicatorReactor * __reactor;
            int __socket;
            char __rx_buffer[ COMMUNICATOR_MAX_PACKET_SIZE ];
            
            hostent * __remote_communicator;
            sockaddr_in __server_addr;
            CommandParser * __parser;
            bool __connected;
            
            std::mutex __transmitt_mutex;
            deque<AbstractCommand * > __transmitt_queue;
            
            void __transmitt();
            void __receiv();
            void __read_from_socket();
            
            void __try_open_socket();
            void __close_socket();
            
        
    }; // CommunicatorClient
    
}; // xenon
