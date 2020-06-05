// *********************************************************************************************************************
// *                                                    Фрейм как таковой.                                             *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 05 jun 2020 at 07:49 *
// *********************************************************************************************************************
#pragma once

#include <map>
#include <boost/any.hpp>

namespace xenon {
    
    class AbstractFrame {
        
        public:
            
            AbstractFrame();
            virtual ~AbstractFrame() = default;
            
        protected:
            
        private:

            std::map<std::string, boost::any> __attributes;
            
    };
    
}; // namespace xenon
