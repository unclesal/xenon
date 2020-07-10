// *********************************************************************************************************************
// *                      Фрейм проверки на исполнительном старте на предмет ранее взлетевшего самолета                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 jul 2020 at 11:16 *
// *********************************************************************************************************************
#pragma once
#include "state_frame.h"

namespace xenon {
    
    class LUBeforeTakeOff: public StateFrame {
        
        public:
            
            LUBeforeTakeOff( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment );
            ~LUBeforeTakeOff() override = default;
            
            void update( CmdAircraftCondition * cmd ) override;
            
        protected:
            
        private:
            
    };
    
};

