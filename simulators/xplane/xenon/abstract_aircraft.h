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
            
            /**
             * @short Common observing for any aircrafts, both user's and fake.
             */
            
//            virtual void observe() override;
//            virtual void to_json(JSON & json) override;
//            virtual void from_json( JSON & json ) override;
//            virtual void set( CommandSet & cmd );



        protected:
        private:
                        
            
    }; // class AbstractAircraft

};
