// *********************************************************************************************************************
// *                      Любой объект, который существует внутри X-Plane и может перемещаться                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:45 *
// *********************************************************************************************************************
#pragma once

#include "structures.h"

namespace xenon {

    class AbstractVehicle {

        public:

            AbstractVehicle();
            virtual ~AbstractVehicle() = default;

            virtual position_t get_position() = 0;
            virtual void set_position( const position_t & position ) = 0;

            virtual location_t get_location();
            virtual void set_location( const location_t & location );
            virtual void set_rotation( const rotation_t & rotation ) {};

            virtual void observe() {};
            // virtual void control( float elapse_since_last_call ) {};

        protected:


    };

}; // namespace xenon