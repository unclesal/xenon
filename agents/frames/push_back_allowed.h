// *********************************************************************************************************************
// *                       Запрет выталкивания, если рядом уже есть самолет, который двигается                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 07 jul 2020 at 13:21 *
// *********************************************************************************************************************
#pragma once

#include "state_frame.h"
namespace xenon {
    
    class PushBackAllowed : public StateFrame {
        
        public:
            
            PushBackAllowed( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment );
            ~PushBackAllowed() override = default;
            
            void update() override;
            
        protected:
            
        private:
            
    };
    
}; // namespace xenon
