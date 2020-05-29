// *********************************************************************************************************************
// *                                            Обертка сверху над журналами                                           *
// *                              Сообщения (для отладки или об ошибке) надо куда-то выводить.                         *
// *                                Но путь вывода - совсем разный, внутри X-Plane и в демонах                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 may 2020 at 13:45 *
// *********************************************************************************************************************

#pragma once


#ifdef INSIDE_XPLANE

#include "../simulators/xplane/xenon/xplane.hpp"

#else

#include <string>
#include <iostream>

#endif

namespace xenon {
    
    class Logger {
        
        public:
                        
            static void log( const std::string & message ) {
#ifdef INSIDE_XPLANE
                XPlane::log( message );
#else
                if ( file_name.empty() ) {
                    std::cout << message << std::endl;
                } else {
                    std::cerr << "Logger::log(), file_name=" << file_name << " not released!" << std::endl;
                };
#endif                
            };
            
            static std::string file_name;
            
        protected:
            
        private:
            
    };

};
