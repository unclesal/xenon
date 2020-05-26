// *********************************************************************************************************************
// *          Класс, который может чего-нибудь устанавливать и чего-нибудь считывать - внутри симулятора X-Plane.      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 24 sep 2019 at 02:51 *
// *********************************************************************************************************************

#pragma once

#include "XPLMDataAccess.h"
#include "CommandSet.h"

namespace xenon {
    
    class AbstractObservedObject {
        
        public:

        /**
         * @short Структура для внутри-игровой позиции объекта.
         */

        struct position_t {
            double x;
            double y;
            double z;
        };

        AbstractObservedObject();
        // AbstractObservedObject( const AbstractObservedObject & aoo );
        virtual ~AbstractObservedObject() = default;
        virtual void observe() = 0;
        // virtual void control(float elapsed_since_last_call);

        // position_t get_position();
        // virtual void set_position( position_t & position );
            
        protected:

            // void _set_as_float( CommandSet & cmd, XPLMDataRef ref );
            // void _set_as_int( CommandSet & cmd, XPLMDataRef ref );
    };
    
};
