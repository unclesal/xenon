// *********************************************************************************************************************
// *                        Фрейм ожидания на предварительном старте, если исполнительный занят                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 jul 2020 at 12:18 *
// *********************************************************************************************************************
#pragma once

#include "state_frame.h"

namespace xenon {
    
    class HpLuOccupated : public StateFrame {
        
        public:
            
            HpLuOccupated(
                BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment
            );
            ~HpLuOccupated() override = default;
            
            void update( CmdAircraftCondition * cmd ) override;
            
        protected:
            
        private:
            
    };
    
}; // namespace xenon

