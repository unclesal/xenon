// *********************************************************************************************************************
// *                             Предварительный старт: если кто-то в стадии посадки, то ждем.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 jul 2020 at 14:50 *
// *********************************************************************************************************************
#pragma once

#include "state_frame.h"

namespace xenon {
    
    class HpSomeOneLanding : public StateFrame {
    
        public:
            
            HpSomeOneLanding( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment );
            ~HpSomeOneLanding() override = default;
            
            void update( CmdAircraftCondition * cmd ) override;
            
        protected:
            
        private:
        
    };
}; // namespace xenon
