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
#include "xplane.hpp"
#include "CommandSet.h"
#include "abstract_vehicle.h"

using namespace std;

namespace xenon {

    class AbstractAircraft : public AbstractVehicle {
        
        friend class AircraftAbstractAction;

        /**
         * Абстрактрый самолет, как пользовательский, так и внутри симулятора.
         */
        
        public:

            AbstractAircraft();
            // AbstractAircraft(const AbstractAircraft & aacf);
            virtual ~AbstractAircraft() override = default;    
            
            // Освещение вкл-выкл
            virtual void set_taxi_lites(bool on) {};
            virtual void set_landing_lites(bool on) {};
            virtual void set_beacon_lites(bool on) {};
            virtual void set_strobe_lites(bool on) {};
            virtual void set_nav_lites(bool on) {};
            
            virtual void move( float meters ) {};

        protected:
            
            deque<waypoint_t> _flight_plan;
            virtual void _action_finished( void * action ) = 0;

        private:
                        
            
    }; // class AbstractAircraft

};
