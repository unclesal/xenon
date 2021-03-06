// *********************************************************************************************************************
// *                                         The transition from XPlane API to C ++ basis.                             *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 mar 2019 at 15:34 *
// *********************************************************************************************************************

#pragma once

// System includes.
#include <map>
#include <vector>
#include <mutex>

// X-Plane plugin SDK includes
#include "XPLMDefs.h"
#include "XPLMDataAccess.h"

// My own includes
// #include "XNetworking.h"
#include "abstract_vehicle.h"
#include "user_aircraft.h"
#include "bimbo_aircraft.h"
#include "XAmbient.h"
// #include "XSetReactor.h"
#include "airport.h"
#include "abstract_command.h"
#include "connected_communicator.h"
#include "connected_communicator_reactor.h"

#include "cmd_query_around.h"
#include "cmd_aircraft_condition.h"
#include "cmd_flight_plan.h"
#include "cmd_waypoint_reached.h"

namespace xenon {

    class XPlanePlugin : public ConnectedCommunicator, public ConnectedCommunicatorReactor {

        public:

            XPlanePlugin( XPLMPluginID & this_plugin_id );
            virtual ~XPlanePlugin();

            void enable();
            void disable();
            void handle_message(XPLMPluginID from, int messageID, void * ptrParam);
            
            /**
             * @short Observe (get, store, transmitt to network e.t.c.) aircrafts positions and states.
             */

            void observe_user_aircraft();
            // void observe_autopilot();
            // void observe_ambient();

            /**
             * @short Наблюдение и управление самолетами AI-трафика.
             *
             * @param elapsed_since_last_call Время в секундах, прошедшее с момента последнего вызова процедуры.
             * Используется для расчета промежуточных состояний самолета на основании его положения, скорости
             * и ускорения.
             */
            // void control_of_bimbo_aircrafts( float elapsed_since_last_call );

            // void set( CommandSet & cmd ) override;

            [[nodiscard]] bool is_enabled() const {
                return __enabled;
            }
            
            void on_connect() override;
            void on_disconnect() override;
            void on_received( void * abstract_command ) override;
            void on_error( std::string message ) override;

        private:

            bool __enabled;
            int __uair_count;
                        
            /**
             * @short This plugin identifier.
             */

            XPLMPluginID __this_plugin_id;

            /**
             * @short Class for networking.
             */

            // XNetworking * __networking;

            /**
             * @short User aircraft.
             *
             * It is always only one, not a collection.
             */

            UserAircraft __user_aircraft;
            XAmbient __ambient;
            // XAutopilotState __autopilot;

            /**
             * @short Мутекс - один на две коллекции агентов.
             * Мутекс - нужен,  потому что пакет по сети может прийти "асинхронно" 
             * по отношению к вызываемым в плагине call-back'ам.
             */
            
            std::mutex __agents_mutex;
            
            // Все движущиеся элементы в игре.
            vector < AbstractVehicle * > __vehicles;
            vector < BimboAircraft * > __bimbos;

            void __init_around();

            /**
             * @short User's aircraft was loaded into simulator.
             */

            void __user_aircraft_was_loaded();

            /**
             * @short AI traffic aircraft was loaded.
             */

            void __ai_controlled_aircraft_was_loaded( int index );
            
            void __command_received( CmdAircraftCondition * cmd );
            void __command_received( CmdFlightPlan * cmd );
            void __command_received( CmdWaypointReached * cmd );
            
            BimboAircraft * __add_one_aircraft( CmdAircraftCondition * cmd );

    }; // class XPlanePlugin
};
