// *********************************************************************************************************************
// *     Некоторая штука (вообще-то "агент"), которая способна общаться с другими такими же "штуками" (агентами)       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 26 may 2020 at 15:03 *
// *********************************************************************************************************************

#pragma once
#include <string>

#include "structures.h"
#include "communicator_interface.h"

namespace xenon {

    class Talker {
        
        public:
            
            /**
             * @short Конструктор.
             * @param uuid уникальный идентификатор этого агента.
             */
            Talker( const talker_t & talker );
            virtual ~Talker() = default;

            talker_t get_talker() {
                return __talker;
            };

        protected:



        private:

            xenon::talker_t __talker;

    }; // class Talker

}; // namespace xenon
