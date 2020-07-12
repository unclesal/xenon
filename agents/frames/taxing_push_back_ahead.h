// *********************************************************************************************************************
// *                          Если при рулении прямо по курсу выполняется выталкивание - ждем.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 jul 2020 at 19:35 *
// *********************************************************************************************************************
#pragma once
#include "state_frame.h"

namespace xenon {
    
    class TaxingPushBackAhead : public StateFrame {
        
        public:
            
            TaxingPushBackAhead( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment );
            ~TaxingPushBackAhead() override = default;
            
            void update( CmdAircraftCondition * cmd ) override;
            
        protected:
            
        private:
        
    };
    
}; // namespace xenon
