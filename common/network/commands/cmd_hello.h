// *********************************************************************************************************************
// *       Команда приветствия. Агент после подключения дает свое местоположение - чтобы понять, что ему видно.        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 12:41 *
// *********************************************************************************************************************
#pragma once

#include "abstract_command.h"
#include "structures.h"

namespace xenon {
    
    class CmdHello: public AbstractCommand {
        
        public:
            
            CmdHello();
            CmdHello( const location_t & location );
            virtual ~CmdHello() override = default;
            
            virtual void to_json( JSON & json ) override;
            virtual void from_json( JSON & json ) override;
            
#ifdef SERVER_SIDE
            virtual void execute_on_server( ConnectedClientListener * client, ClientsListener * server );
#endif

            
        protected:
            
            location_t _location;
            
    }; // class CmdHello
    
}; // namespace xenon

