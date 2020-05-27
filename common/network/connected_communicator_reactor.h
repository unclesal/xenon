// *********************************************************************************************************************
// *                      Интерфейс реакций на события в (якобы уже присоединенном) коммуникатореа                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 26 may 2020 at 15:13 *
// *********************************************************************************************************************
#pragma once

#include <string>

#include "abstract_command.h"

namespace xenon {

    class ConnectedCommunicatorReactor {
        
        public:

            ConnectedCommunicatorReactor() = default;
            virtual ~ConnectedCommunicatorReactor() = default;
                    
            virtual void on_connect() = 0;
            virtual void on_disconnect() = 0;
            virtual void on_received( AbstractCommand * cmd ) = 0;
            virtual void on_error( std::string message ) = 0;
            

    }; // class ConnectedCommunicatorReactor

}; // namespace xenon
