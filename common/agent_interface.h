// *********************************************************************************************************************
// *                                              Интерфейс агента верхнего уровня.                                    *
// *  Нужен для того, чтобы сообщить агенту, что выбранное действие было полностью завершено или изменилось состояние  *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 07 jul 2020 at 01:45 *
// *********************************************************************************************************************
#ifdef INSIDE_AGENT

#pragma once

namespace xenon {

    class AgentInterface {
        
        public:
            virtual void action_started( void * action ) = 0;
            virtual void action_finished( void * action ) = 0;
            virtual void state_changed( void * state ) = 0;

    };
    
}; // namespace xenon

#endif
