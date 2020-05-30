// *********************************************************************************************************************
// *                                          Передача по сети состояния "самоходки"                                   *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 30 may 2020 at 13:06 *
// *********************************************************************************************************************
#pragma once

#include "abstract_command.h"

namespace xenon {
    
    class CmdVehicleCondition : public AbstractCommand {
        
        public:
            
            CmdVehicleCondition();
            CmdVehicleCondition( const vehicle_condition_t & vcl_condition );
            virtual ~CmdVehicleCondition() override = default;
            
            virtual void to_json( JSON & json ) override;
            virtual void from_json( JSON & json ) override;
            
        protected:
            
            vehicle_condition_t _vcl_condition;
            
        private:                        
            
    };
    
}; // namespace xenon
