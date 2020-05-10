// ********************************************************************************************************************
// *                                    Класс, реализующий работу по сети для тестера X-Imi.                          *
// *                                                                                                                  *
// *                                                                      Дата создания файла: 11 февраля 2017, 13:07 *
// ********************************************************************************************************************

#pragma once

#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include <QUdpSocket>
#include <QTime>
#include <QTimer>
#include <QByteArray>

#include "Settings.h"
#include "JSONAble.h"
#include "AircraftState.h"
#include "Ambient.h"
#include "CommandAutopilotState.h"
#include "CommandSet.h"

// Время тикания таймера определения соединенности по сети с X-Plane
#define INTERVAL_CONNECTION_TIMER               500
#define INTERVAL_REOPEN_TIMER                   10000

// Время в мс, по истечении чего мы считаем, что симулятор отсоединился.
#define LAST_RECEIVED_INTERVAL_FOR_DISCONNECT   1000

namespace xenon {

    /**
     * @short Высокоуровневая реализация работы по сети.
     * 
     * "Высокоуровневая" в том смысле, что плагин работает - с "низкоуровневой", написанной на C.
     */

    class XeNetwork : public QObject {

        Q_OBJECT

        signals:

            void signal__got_autopilot_state( CommandAutopilotState & autopilot );
            void signal__got_ambient( Ambient ambient );
            void signal__got_user_aircraft_state( AircraftState & user_aircraft );
            void signal__got_bimbo_aircraft( AircraftState & bimbo_aircraft );
            void signal__xplane_connected();
            void signal__xplane_disconnected();

        public:

            explicit XeNetwork( QObject* parent = nullptr );
            virtual ~XeNetwork() override = default;

            // int send_to_all( QByteArray data );
            // int send_to_all( void * data, int len );

            void reset();

        public slots:

            void slot__need_send( CommandSet & cmd );

        private:

            uint16_t _port;
            QHostAddress _groupAddress;

            /**
             * @short Внутренний таймер.
             * 
             * Таймер шлепает на предмет "соединения". Грустно смотреть на экран, на котором
             * ничего не происходит. С другой стороны, это UDP, собственно факта "соединения"
             * здесь нет. Поэтому соединенность определяем - по таймеру. Единственный смысл
             * этого телодвижения - просто отобразить, есть в природе симулятор или его нет.
             */

            QTimer * __disconnect_timer;
            QTimer * __reopen_timer;

            // Наличие "соединения" с симулятором X-Plane.
            bool __xplane_connected;
            
            // Время получения последнего пакета от X-Plane
            QTime __last_received_from_xplane;
            

            /**
             * @short Приемная UDP сокета.
             */

            QUdpSocket * _rxSocket;

            /**
             * @short Передающая UDP сокета.
             */

            QUdpSocket * _txSocket;

            void __set_xplane_connected();
            void __set_xplane_disconnected();

        private slots:

            /**
             * @short Что-то было получено из сети.
             */

            void slot__something_received();

            /**
             * @short Срабатывание таймера "соединения" с плагином X-Plane.
             */

            void slot__disconnect_timer_fired();
            void slot__reopen_timer_fired();

    };

};

