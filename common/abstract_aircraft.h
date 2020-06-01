// *********************************************************************************************************************
// *                                       An abstract aircraft internal of X-Plane simulator.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 15:46 *
// *********************************************************************************************************************

#pragma once

// System includes.
#include <string>
#include <deque>

// My own includes.
#include "abstract_vehicle.h"

using namespace std;

namespace xenon {

    class AbstractAircraft : public AbstractVehicle {
        
        friend class AbstractAircrafter;

        /**
         * Абстрактрый самолет, как пользовательский, так и внутри симулятора.
         */
        
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
            
            virtual void move( float meters ) {};
            
            virtual void update_from( const aircraft_condition_t & ac );
            
        protected:
            
            deque<waypoint_t> _flight_plan;
            
            aircraft_parameters_t _params;
            
            virtual void _action_finished( void * action ) = 0;            

        private:
                        
                        
            
    }; // class AbstractAircraft

};
