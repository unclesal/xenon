// *********************************************************************************************************************
// *                                      Распознавание полученных по сети команд                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 07:02 *
// *********************************************************************************************************************
#pragma once

#include <vector>
#include <string>
#include <map>

#include "jsonable.h"
#include "abstract_command.h"

namespace xenon {
    
    template<typename T> AbstractCommand * __create_command() { return new T; };
    
    class CommandParser {
        
        typedef std::map< std::string, AbstractCommand * (*)() > map_type;
        
        public:
            
            CommandParser();
            ~CommandParser() = default;
            AbstractCommand * parse( char * buffer, const ssize_t & len, std::string & command_name );            
            
        private:
            
            
            map_type __map;
            
            template<typename T> void __register(std::string const & s) { 
                __map[s] = & __create_command<T>;
            };
            
            AbstractCommand * __create_instance( std::string const & s ) {
                map_type::iterator it = __map.find(s);
                if ( it == __map.end()) return nullptr;
                return it->second();
            };
            
        
    }; // CommandParser
    
}; // xenon
