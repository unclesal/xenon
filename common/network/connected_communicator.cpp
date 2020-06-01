// *********************************************************************************************************************
// *                    Клиентская часть общения с коммуникатором, будем считать, что уже соединенная.                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 05:58 *
// *********************************************************************************************************************
#include "connected_communicator.h"
#include "utils.hpp"
#include "abstract_command_transmitter.hpp"

using namespace xenon;

unsigned int ConnectedCommunicator::__packet_number = 0;

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
    __remote_communicator = nullptr;
    __connected = false;
    
    std::thread transmitter( & ConnectedCommunicator::__transmitt, this );
    transmitter.detach();
    std::thread receiver( & ConnectedCommunicator::__receiv, this );
    receiver.detach();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                 Метод выполнения передающего потока коммуникатора                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::__transmitt() {

    for (;;) {

        usleep(10);
        if (( __socket < 0 ) || ( ! __connected )) continue;

        if ( ! __transmitt_queue.empty() ) {
            // Пока что не используем мутекс: из нулевого элемента деки выбирается все равно
            // только здесь, а передача займет какое-то время и блокировать деку - не хочется.
            
            AbstractCommand * cmd = __transmitt_queue.at(0);                                    
            AbstractCommandTransmitter trx( __socket );
            std::string error;
            if ( ! trx.transmitt( cmd, error ) ) {

                __reactor->on_error(
                    "ERROR: ConnectedCommunicator::__transmitt(): command " + cmd->command_name() 
                    + " was not transmitted because " + error
                );
                sleep( 10 );

            } else {
                
                // Команда была передана полностью, как и планировалось.
                // Соответственно, можно ее удалять из очереди и из памяти.
                __transmitt_mutex.lock();
                __transmitt_queue.pop_front();
                __transmitt_mutex.unlock();
                
                delete( cmd );
                cmd = nullptr;
            }            
        }
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Метод выполнения приемного потока коммуникатора                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::__receiv() {
    for (;;) {
        usleep(10);
        __try_open_socket();
        __read_from_socket();
    };
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Попытка открытия сокеты                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::__try_open_socket() {
    
    if ( ( __socket < 0 ) || ( ! __connected ) ) {            
        
        // Попытка открытия сокеты. Причем, может быть такая ситуация, что сама сокета-то 
        // открыта, но вот соединиться мы не смогли. Учитываем эту ситуацию.        
        __connected = false;
        if ( __socket < 0 ) __close_socket();                            
        
        __socket = socket(AF_INET, SOCK_STREAM, 0);
        if ( __socket < 0 ) return;
        __remote_communicator = gethostbyname( get_communicator_host().c_str() );
        if ( ! __remote_communicator ) {
            __close_socket();      
            __reactor->on_error(
                "ConnectedCommunictor::__receiv(), no such host " + get_communicator_host()
            );
            sleep(60);
            return;
        }
        
        memset( &__server_addr, 0, sizeof( __server_addr ) );
        __server_addr.sin_family = AF_INET;
        bcopy((char *) __remote_communicator->h_addr, (char *) & __server_addr.sin_addr.s_addr, __remote_communicator->h_length);            
        __server_addr.sin_port = htons( get_communicator_port() );
        
        if ( ::connect( __socket, (struct sockaddr * ) & __server_addr, sizeof(__server_addr)) < 0) {
            if ( errno != ECONNREFUSED ) // connection refused
            {
                __reactor->on_error(
                    "ConnectedCommunicator::connect: " + std::to_string( errno ) 
                    + ", message=" + std::string( strerror( errno ))
                );
            }
            __close_socket();
            sleep(10);
            return;
        }

        // Если здесь остались - мы соединены.
        __connected = true;
        __reactor->on_connect();

    };
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *               Запрос от клиентской стороны коммуникатору. Точнее, это только складывание в очередь                *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::request( AbstractCommand * cmd ) {
        
    // Номера передаваемых команд по порядку.
    
    ConnectedCommunicator::__packet_number ++;
    if ( ConnectedCommunicator::__packet_number >= 0xfff0 ) ConnectedCommunicator::__packet_number = 0;
    cmd->__packet_number = ConnectedCommunicator::__packet_number;
    
    // Системное время - на момент складывания пакета в очередь.
    // Т.е. именно тогда, когда координаты были валидны. Задержка
    // на нахождение в очереди перед передачей будет учтена сама собой.
    
    cmd->_vcl_condition.agent_system_time_ms = xenon::get_system_time_ms();

    __transmitt_mutex.lock();
    __transmitt_queue.push_back( cmd );
    __transmitt_mutex.unlock();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Чтение с сокеты (блокирует поток)                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::__read_from_socket() {
        
    if ( __socket < 0 ) return;
    memset( __rx_buffer, 0, sizeof( __rx_buffer) );
    ssize_t bytes = ::read( __socket, __rx_buffer, sizeof( __rx_buffer ) );
    if ( bytes <= 0 ) {        
        // __reactor->on_error("ConnectedCommunicator::__read_from_socket: received " + std::to_string(bytes));
        __close_socket();
        return;
    }

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
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Закрытие сокеты                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void ConnectedCommunicator::__close_socket() {
    
    __remote_communicator = nullptr;
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
