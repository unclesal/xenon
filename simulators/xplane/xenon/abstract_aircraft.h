// *********************************************************************************************************************
// *                                       An abstract aircraft internal of X-Plane simulator.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 15:46 *
// *********************************************************************************************************************

#pragma once

// System includes.
#include <string>

// My own includes.
#include "xplane.hpp"
#include "CommandSet.h"
#include "abstract_vehicle.h"

using namespace std;

namespace xenon {

    class AbstractAircraft : public AbstractVehicle {

        /**
         * Абстрактрый самолет, как пользовательский, так и внутри симулятора.
         */
        
        public:

            AbstractAircraft();
            // AbstractAircraft(const AbstractAircraft & aacf);
            virtual ~AbstractAircraft() override = default;
            
            virtual void does_finished( void * action ) = 0;


        protected:
        private:
                        
            
    }; // class AbstractAircraft

};
