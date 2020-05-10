// *********************************************************************************************************************
// *                                  The reaction on CommandSet receiving into X-Plane simulator.                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 sep 2019 at 21:82 *
// *********************************************************************************************************************
#pragma once

#include "CommandSet.h"

namespace xenon {
    
    class XSetReactor {
        public:
            virtual void set(CommandSet & cmd) = 0;
    };
    
};
