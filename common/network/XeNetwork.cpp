// ********************************************************************************************************************
// *                            Класс, реализующий работу по сети для системы моделирования X-Imi.                    *
// *                                                                                                                  *
// *                                                                      Дата создания файла: 11 февраля 2017, 13:07 *
// ********************************************************************************************************************
#include "XeNetwork.h"

using namespace xenon;

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                                     Конструктор.                                                 *
// *                                                                                                                  *
// ********************************************************************************************************************

XeNetwork::XeNetwork ( QObject* parent )
    : QObject ( parent )     
{
    _rxSocket = nullptr;
    __xplane_connected = false;
    _groupAddress = QHostAddress("0.0.0.0");

    // Это, собственно, пока особо не важно, т.к. соединения с x-plane все равно еще нет.
    __last_received_from_xplane = QTime::currentTime();

    // Тикалка для определения "соединенности" с симулятором.

    __disconnect_timer = new QTimer( this );
    QObject::connect( __disconnect_timer, SIGNAL( timeout() ), this, SLOT( slot__disconnect_timer_fired() ) );

    // Тикалка для пересоздания приемной сокеты.
    __reopen_timer = new QTimer( this );
    QObject::connect( __reopen_timer, SIGNAL( timeout() ), this, SLOT( slot__reopen_timer_fired() ) );

    // Передающая сокета. Она создается один раз, reset() на нее не влияет,
    // потому что она установки использует - на момент передачи данных. Какие 
    // будут к тому времени установлены значения - такие и будут использованы ей.

    _txSocket = new QUdpSocket( this );
    
    // После того, как все полностью создали, проводим инициализацию приемника.
    reset();
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                                Сброс сетевых функций.                                            *
// *                                                                                                                  *
// ********************************************************************************************************************

void XeNetwork::reset() {

    // В этом месте принимаются установки, которые имеет программа в отнношении сети.
    _port = NETWORK_PORT;
    _groupAddress = QHostAddress( NETWORK_MULTICAST_GROUP );

    // На время работы процедуры запрещаем таймер
    __disconnect_timer->stop();
    __reopen_timer->stop();

    if ( _rxSocket ) {
        // Если сокета существует - удаляем ее.
        try {
            QObject::disconnect( _rxSocket, SIGNAL(readyRead()), this, SLOT(slot__something_received()) );
            _rxSocket->close();
            _rxSocket->deleteLater();
        } catch ( ... ) {
            qDebug() << "Network::reset(): ошибка при удалении старой приемной сокеты";
        }
    }

    // Конструируем приемную сокету.
    _rxSocket = new QUdpSocket(this);
    QObject::connect( _rxSocket, SIGNAL(readyRead()), this, SLOT(slot__something_received()) );
    _rxSocket->bind(QHostAddress::AnyIPv4, _port, QUdpSocket::ShareAddress );
    _rxSocket->joinMulticastGroup( _groupAddress );

    __reopen_timer->start( INTERVAL_REOPEN_TIMER );

}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                     Установить соединенность с симулятором                                       *
// *                                                                                                                  *
// ********************************************************************************************************************

void XeNetwork::__set_xplane_connected() {

    __xplane_connected = true;
    emit signal__xplane_connected();
    __reopen_timer->stop();
    __disconnect_timer->start( INTERVAL_CONNECTION_TIMER );

}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                    Установить рассоединенность с симулятором                                     *
// *                                                                                                                  *
// ********************************************************************************************************************

void XeNetwork::__set_xplane_disconnected() {
    __xplane_connected = false;
    emit signal__xplane_disconnected();
    reset();
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                           Срабатывание таймера определения "соединенности" с симулятором.                        *
// *                                                                                                                  *
// ********************************************************************************************************************

void XeNetwork::slot__disconnect_timer_fired() {

    QTime currentTime = QTime::currentTime();
    if ( __xplane_connected ) {
        if ( __last_received_from_xplane.msecsTo( currentTime ) > LAST_RECEIVED_INTERVAL_FOR_DISCONNECT ) {
            // Наступило время "рассоединиться" с X-Plane.
            __set_xplane_disconnected();
        }
    }
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *         Заполнение команды достоверной известным для данной версии программного обеспечения числом байт.         *
// *                                                                                                                  *
// ********************************************************************************************************************
/*
void ximi::QtNetwork::_fillCommand ( void * dst, int dstLen, QByteArray & datagram ) {

    void * src = datagram.data();
    int srcLen = datagram.size();

    // Приемная структура заполняется нулями. Это позволит обезопаситься от неверных данных при ситуации,
    // когда размер приемной структуры вдруг оказался меньше реально принятого значения (то есть в ходе
    // коррекции протокола структуры были уменьшены по длине).

    memset( dst, 0, dstLen );

    // Размер - минимальный достоверно известный для данной версии протокола,
    // для той команды, которая есть у этого экземпляра программного обеспечения.

    int l = dstLen;
    if ( l > srcLen ) l = srcLen;
    memcpy( dst, src, l );

}
*/
// ********************************************************************************************************************
// *                                                                                                                  *
// *                                             Что-то было получено из сети.                                        *
// *                                                                                                                  *
// ********************************************************************************************************************

void XeNetwork::slot__something_received() {

    while ( _rxSocket->hasPendingDatagrams()) {

        QByteArray datagram;
        int datagram_len = static_cast<int>( _rxSocket->pendingDatagramSize() );
        datagram.resize( datagram_len );
        QHostAddress host;
        quint16 port;
        int received = static_cast<int>(_rxSocket->readDatagram(datagram.data(), datagram.size(), & host, & port ));
        if (received > 0 ) {

            try {
                // Парзинг полученной по сети команды.

                JSON json = JSON::parse(datagram.data());
                string command = json.value("command_name", "");

                // Изменение свойств соединенности делаем только в том
                // случае, если не было ошибок парзинга.

                __last_received_from_xplane = QTime::currentTime();
                if ( ! __xplane_connected ) {
                    __set_xplane_connected();
                }


                if ( command == CommandAutopilotState::COMMAND_NAME) {

                    // Была получена команда состояния автопилота.
                    CommandAutopilotState ap;
                    ap.from_json( json );
                    emit signal__got_autopilot_state( ap );

                } else if ( command == Ambient::COMMAND_NAME ) {

                    // Была получена команда состояния окружающей среды.
                    Ambient ab;
                    ab.from_json( json );
                    emit signal__got_ambient( ab );

                } else if ( command == AircraftState::COMMAND_NAME ) {

                    // Была получена команда состояния самолета.
                    AircraftState ac;
                    ac.from_json( json );
                    if ( ac.aircraft_index() == 0 ) emit signal__got_user_aircraft_state( ac );
                    else emit signal__got_bimbo_aircraft( ac );
                } else if ( command == CommandSet::COMMAND_NAME ) {
                    // CommandSet - ничего не делаем.
                } else {
                    qDebug() << "XeNetwork::slot__something_received(): unhandled command " << command.c_str();
                    qDebug() << "Packet: " << datagram.data();
                }
            } catch ( JSON::exception & e ) {
                qDebug() << "XeNetwork::slot__something_received(), JSON exception: " << e.what();
            } catch ( std::exception & e ) {
                qDebug() << "XeNetwork::slot__something_received():" << e.what();
            } catch ( ... ) {
                qDebug() << "XeNetwork::slot__something_received(), unhandled exception.";
            }
        }

    }
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                       Отправить в сеть произвольную команду.                                     *
// *                                                                                                                  *
// ********************************************************************************************************************
/*
int XeNetwork::send_to_all( QByteArray data ) {
    qint64 l = _txSocket->writeDatagram( data.data(), data.size(), _groupAddress, _port );
    int i = static_cast<int>(l);
    return i;
}
*/
// ********************************************************************************************************************
// *                                                                                                                  *
// *                         Вариант отправки произвольной структуры (через указатель и длину )                       *
// *                                                                                                                  *
// ********************************************************************************************************************
/*
int XeNetwork::send_to_all( void * data, int len ) {
    qint64 l = _txSocket->writeDatagram( static_cast< const char * >( data ), len, _groupAddress, _port );
    int i = static_cast<int>(l);
    return ( i );
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                    Слот отправки в сеть команды установки какой-либо переменной симулятора.                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void XeNetwork::slot__need_send( CommandSet & cmd ) {

    if ( ! __xplane_connected ) {
        qDebug() << "XeNetwork::slot__need_send() while not connected.";
        return;
    }

    JSON json;
    cmd.to_json( json );
    std::string str = json.dump();
    QByteArray byte_array = QString::fromStdString( str ).toUtf8();

    // send_to_all( byte_array );
    // _txSocket->writeDatagram( byte_array.data(), byte_array.size(), __xplane_address, NETWORK_PORT );
    _txSocket->writeDatagram( byte_array.data(), byte_array.size(), _groupAddress, _port );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Срабатывание таймера пересоздания сокеты                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void XeNetwork::slot__reopen_timer_fired() {
    reset();
}
