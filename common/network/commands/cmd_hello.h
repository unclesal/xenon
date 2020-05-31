// *********************************************************************************************************************
// *       Команда приветствия. Агент после подключения дает свое местоположение - чтобы понять, что ему видно.        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 12:41 *
// *********************************************************************************************************************
#pragma once

#include "abstract_command.h"
#include "structures.h"

namespace xenon {
    
    /**
     * @short Команда приветствия.
     * Назначение команды - это определить в коммуникаторе, кто кого "слышит" на основании расстояний между агентами.
     */

    class CmdHello: public AbstractCommand {
        
        public:
            
            CmdHello();
            CmdHello( const vehicle_condition_t & vcl_condition );
            virtual ~CmdHello() override = default;
            
        protected:
            
        private:
            
    }; // class CmdHello
    
}; // namespace xenon

