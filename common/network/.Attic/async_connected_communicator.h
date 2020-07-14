// *********************************************************************************************************************
// *                                     Асинхронная реализация работы с коммуникатором                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 jul 2020 at 12:57 *
// *********************************************************************************************************************
#pragma once

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <deque>
#include <mutex>

#include "connected_communicator_reactor.h"
#include "abstract_command.h"
#include "abstract_command_transmitter.hpp"

using boost::asio::ip::tcp;

namespace xenon {
    
    class AsyncConnectedCommunicator {
        
        public:
            
            AsyncConnectedCommunicator( boost::asio::io_service & io_service, ConnectedCommunicatorReactor * reactor );
            ~AsyncConnectedCommunicator() = default;
            
            void request( AbstractCommand * cmd );
            void disconnect();
            
            void start( tcp::resolver::iterator endpoint_iter );
            void stop();
            
            bool is_connected() {
                return __socket.is_open();
            };
            
        protected:
            
        private:
            
            std::deque< AbstractCommand * > __transmitt_queue;
            std::mutex __transmitt_mutex;
            
            ConnectedCommunicatorReactor * __reactor;            
            bool __stopped;
            
            tcp::socket __socket;

            boost::asio::streambuf __input_buffer;
            boost::asio::streambuf __output_buffer;
            boost::asio::deadline_timer __deadline;
            
            void __start_connect(tcp::resolver::iterator endpoint_iter);
            void __handle_connect( const boost::system::error_code & ec, tcp::resolver::iterator endpoint_iter );
            void __start_read();
            void __handle_read( const boost::system::error_code & ec );
            
            void __start_write();
            void __handle_write( const boost::system::error_code & ec );
            
            void __check_deadline();
        
    };
    
};

