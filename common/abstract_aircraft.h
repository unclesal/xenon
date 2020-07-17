// *********************************************************************************************************************
// *                                       An abstract aircraft internal of X-Plane simulator.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 15:46 *
// *********************************************************************************************************************

#pragma once

// System includes.
#include <string>
#include <deque>
#include <mutex>

// My own includes.
#include "abstract_vehicle.h"
#include "flight_plan.h"

using namespace std;

namespace xenon {

    /**
     * @short Абстрактрый самолет, как пользовательский, так и внутри симулятора.
     */

    class AbstractAircraft : public AbstractVehicle {
                
        public:

            AbstractAircraft();
            virtual ~AbstractAircraft() override = default;    
            
            aircraft_condition_t acf_condition;
            
            // Освещение вкл-выкл
            virtual void set_taxi_lites(bool on) {};
            virtual void set_landing_lites(bool on) {};
            virtual void set_beacon_lites(bool on) {};
            virtual void set_strobe_lites(bool on) {};
            virtual void set_nav_lites(bool on) {};
            
            virtual void set_gear_down( bool down ) { };
            virtual void set_reverse_on( bool on ) { };
            virtual void set_flaps_position( const float & position ) { };
            virtual void set_thrust_position( const float & position ) {};
            virtual void set_speed_brake_position( const float & value ) {};
            
            /**
             * @short Стереть точки полетного плана вплоть до данной.
             * Реакция на тот факт, что в "основном" агенте была достигнута
             * определенная точка полетного плана. В отличие от прямого обращения
             * к FlightPlan, здесь задействован "самолетный мутекс".
             */
            inline void erase_up_to( const uint16_t & npp ) {
                _acf_mutex.lock();
                flight_plan.erase_up_to( npp );
                _acf_mutex.unlock();
                
                Logger::log("AbstractAircraft::erase_up_to " + to_string ( npp ));
                for ( int i=0; i<flight_plan.size(); i++ ) {
                    auto wp = flight_plan.get( i );
                    Logger::log(
                        "   " + to_string(wp.npp) + ", " + wp.name + ", " + waypoint_to_string( wp.type ) + ", " + action_to_string( wp.action_to_achieve ) 
                    );
                };
                
            };

            void set_flight_plan( const FlightPlan & fp ) {
                _acf_mutex.lock();
                flight_plan.clear();
                flight_plan = fp;
                _acf_mutex.unlock();
                
                Logger::log("FlightPlan changed.");
                for ( int i=0; i<flight_plan.size(); i++ ) {
                    auto wp = flight_plan.get( i );
                    Logger::log(
                        "   " + to_string( wp.npp ) + ", " + wp.name + ", " + waypoint_to_string( wp.type ) + ", " + action_to_string( wp.action_to_achieve ) 
                    );
                };
                
            };
            
            virtual void move( float meters ) {};
            
            virtual void update_from( vehicle_condition_t & vc, aircraft_condition_t & ac );
            virtual void prepare_for_taxing( const deque< waypoint_t > & taxi_way ) {};
            
            FlightPlan flight_plan;
            
            const aircraft_parameters_t & parameters() { return _params; };            

        protected:
            
            aircraft_parameters_t _params;
            std::mutex _acf_mutex;

        private:
                        
                        
            
    }; // class AbstractAircraft

};
