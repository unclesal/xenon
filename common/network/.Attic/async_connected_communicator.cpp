// *********************************************************************************************************************
// *                                     Асинхронная реализация работы с коммуникатором                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 jul 2020 at 12:59 *
// *********************************************************************************************************************

#include "async_connected_communicator.h"

using namespace xenon;
using namespace boost::placeholders;
using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AsyncConnectedCommunicator::AsyncConnectedCommunicator( boost::asio::io_service & io_service, ConnectedCommunicatorReactor * reactor ) 
    : __socket( io_service ),
    __deadline( io_service )
{
    
    __reactor = reactor;
    __stopped = false;       
    
}

// *********************************************************************************************************************
// *                                                    start                                                          *
// *                                                                                                                   *
// *                 Called by the user of the client class to initiate the connection process.                        *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::start( tcp::resolver::iterator endpoint_iter ) {
    
    // Start the connect actor.
    __start_connect( endpoint_iter );

    // Start the deadline actor. You will note that we're not setting any
    // particular deadline here. Instead, the connect and input actors will
    // update the deadline prior to each asynchronous operation.
    __deadline.async_wait(boost::bind( &AsyncConnectedCommunicator::__check_deadline, this));    
    
}

// *********************************************************************************************************************
// *                                                       stop                                                        *
// *                                                                                                                   *
// *                      This function terminates all the actors to shut down the connection. It                      *
// *                     may be called by the user of the client class, or by the class itself in                      *
// *                           response to graceful termination or an unrecoverable error.                             *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::stop() {
    
    cout << "!!! AsyncConnectedCommunicator::stop !!!";
    __stopped = true;
    __socket.close();
    __deadline.cancel();
    __reactor->on_disconnect();
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Start connect                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::__start_connect( tcp::resolver::iterator endpoint_iter ) {
    
    if ( endpoint_iter != tcp::resolver::iterator() ) {
                

        // Set a deadline for the connect operation.
        __deadline.expires_from_now(boost::posix_time::seconds(30));

        // Start the asynchronous connect operation.
                        
        cout << "Trying to connect... " << endpoint_iter->endpoint() << endl;
        
        __socket.async_connect(endpoint_iter->endpoint(),
            boost::bind( 
                &AsyncConnectedCommunicator::__handle_connect,
                this, _1, endpoint_iter
            )
        );
        
    } else {
        
        // There are no more endpoints to try. Shut down the client.        
        stop();                
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Handle connect                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::__handle_connect( const boost::system::error_code & ec, tcp::resolver::iterator endpoint_iter ) {
    
    if ( __stopped) return;

    // The async_connect() function automatically opens the socket at the start
    // of the asynchronous operation. If the socket is closed at this time then
    // the timeout handler must have run first.
    if ( !__socket.is_open() ) {
        
        Logger::log("Connect timed out");

        // Try the next available endpoint.
        __start_connect( ++endpoint_iter );
    }

    // Check if the connect operation failed before the deadline expired.
    else if (ec) {
        Logger::log( std::string("Connect error: ") + ec.message() );

        // We need to close the socket used in the previous connection attempt
        // before starting a new one.
        __socket.close();

        // Try the next available endpoint.
        __start_connect( ++endpoint_iter );
    }

    // Otherwise we have successfully established a connection.
    else {
        
        __reactor->on_connect();
        Logger::log("Connected!!!");
        // Start the input actor.
        __start_read();
        
        // Start the heartbeat actor.
        
        __start_write();
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Start read actor                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::__start_read() {
    
    // Set a deadline for the read operation.
    __deadline.expires_from_now(boost::posix_time::seconds(30));

    // Start an asynchronous operation to read a newline-delimited message.
    boost::asio::async_read(__socket, __input_buffer, boost::bind(&AsyncConnectedCommunicator::__handle_read, this, _1));
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Handle read                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************
  
void AsyncConnectedCommunicator::__handle_read( const boost::system::error_code & ec ) {
    
    if ( __stopped ) return;

    if ( !ec ) {
        
        Logger::log("Somewhat received.");
        /*
        // Extract the newline-delimited message from the buffer.
        std::string line;
        std::istream is(&__input_buffer);
        std::getline(is, line);

        // Empty messages are heartbeats and so ignored.
        if (!line.empty()) {
            std::cout << "Received: " << line << "\n";
        }
        */

        __start_read();
    } else {
        Logger::log("AsyncConnectedCommunicator::error on receive: " + ec.message());

        stop();
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Start write into socket                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::__start_write() {
    
    if ( __stopped) return;
    
    if ( ! __transmitt_queue.empty() ) {
        
        // Пока что не используем мутекс: из нулевого элемента деки выбирается все равно
        // только здесь, а передача займет какое-то время и блокировать деку - не хочется.
                
        AbstractCommand * cmd = __transmitt_queue.at(0);        
        JSON json;
        cmd->to_json( json );                
        std::vector<std::uint8_t> trx = JSON::to_ubjson(json);
        
        std::ostream of( &__output_buffer );
        uint16_t len = trx.size();
        of.write( (char*) &len, sizeof( len ));
        
        // std::ostream_iterator<std::uint8_t> output_iterator(of);
        // std::copy(trx.begin(), trx.end(), output_iterator);
        
        of.write( (char * ) trx.data(), trx.size() );
              
        // Start an asynchronous operation to send a heartbeat message.
        boost::asio::async_write( 
            __socket, __output_buffer,
            boost::bind( &AsyncConnectedCommunicator::__handle_write, this, _1)
        );        
    }    
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Handle write                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************
void AsyncConnectedCommunicator::__handle_write( const boost::system::error_code & ec ) {
    
    if (__stopped) return;
    
    if ( ! ec ) {
        
        Logger::log("Command was transmitted successfully");
        // Команда была передана полностью, как и планировалось.
        // Соответственно, можно ее удалять из очереди и из памяти.
        
        __transmitt_mutex.lock();
        AbstractCommand * cmd = __transmitt_queue.at( 0 );
        delete( cmd );
        cmd = nullptr;
        __transmitt_queue.pop_front();
        __transmitt_mutex.unlock();
        
    } else {
        
        Logger::log( "Error on write: " + ec.message());
        stop();
        
    }    
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Check deadline                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::__check_deadline() {
    
    if (__stopped) return;

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if ( __deadline.expires_at() <= deadline_timer::traits_type::now()) {
        
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        __socket.close();

        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        __deadline.expires_at(boost::posix_time::pos_infin);
    }

    // Put the actor back to sleep.
    __deadline.async_wait(boost::bind(&AsyncConnectedCommunicator::__check_deadline, this));
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Принудительное рассоединение коммуникатора                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::disconnect() {
    stop();
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Требование передачи команды в сеть                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void AsyncConnectedCommunicator::request( AbstractCommand * cmd ) {
    
    __transmitt_mutex.lock();
    __transmitt_queue.push_back( cmd );
    __transmitt_mutex.unlock();
    
    __start_write();
    
}



