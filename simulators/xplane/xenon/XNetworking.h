// *********************************************************************************************************************
// *                                                Main clas for networking                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 mar 2019 at 19:57 *
// *********************************************************************************************************************

#pragma once

// System includes.

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <stdint.h>
#include <time.h>

#ifdef __WIN32__
    // Include files for MS Windows
    #include <winsock.h>
#else
    // Include files for Linux
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <errno.h>
#endif

// My own include files.
#include "jsonable.h"
#include "CommandSet.h"
#include "XSetReactor.h"

namespace xenon {

    class XNetworking {

        public:

            static XNetworking * create();
            ~XNetworking();

            /**
             * @short Send network packet using multicast socket.
             */

            void send_to_all( void * cmd, int len );

            /**
             * @short Send a command to network using multicast socket.
             */

            void send_to_all( JSONAble * command );

            void receive();
            void set_setter( XSetReactor * setter ) {
                __setter = setter;
            }

        private:

            XNetworking();

            // Char buffer for in-memory printing a messages before logging it.
            char __out[1024];

            // Multicast transmitt socket descriptor.
            int __transmitter;

            // Address structure for multicast socket.
            sockaddr_in __transmitter_address;

            int __receiver;
            char * __rx_buffer[ 4  * 1024 ];
            pthread_t __receiver__thread_id;

            XSetReactor * __setter;

            // bool __prepare_socket_for_reuse( int & socket );

            void __close_socket( int & socket );
            void __init_receiver();
    };
};
