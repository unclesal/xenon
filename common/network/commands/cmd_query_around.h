// *********************************************************************************************************************
// *                     Запросить у сервера всех агентов, которые может "слышать" данный агент.                       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 31 may 2020 at 14:41 *
// *********************************************************************************************************************
#pragma once

#include "abstract_command.h"

namespace xenon {
    
    class CmdQueryAround : public AbstractCommand {
        
        public:
            
            CmdQueryAround();
            CmdQueryAround( const vehicle_condition_t & vcl_condition );
            virtual ~CmdQueryAround() override = default;
            
            void to_json( JSON & json ) override;
            void from_json( JSON & json ) override;
            
#ifdef SERVER_SIDE
            void execute_on_server( ConnectedClientCore * client, ClientsListener * server ) override;
#endif
        protected:
            
        private:
            
    };
}; // namespace xenon
