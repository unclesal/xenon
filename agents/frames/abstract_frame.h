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
            
            typedef std::map< std::string, boost::any > frame_attributes_t ;
            
            AbstractFrame();
            virtual ~AbstractFrame() = default;
            
            /**
             * @short Есть ли такой атрибут?
             */
            inline bool exists( const std::string & key, frame_attributes_t::iterator & it ) {
                it = __attributes.find( key );
                return ( it != __attributes.end() );
            };
            
            template <class T> bool get( const std::string & key, T & value ) {
                frame_attributes_t::iterator it;
                if ( ! exists( key, it ) ) return false;
                try {
                    value = boost::any_cast< T > ( it->second );
                    return true;
                } catch ( const boost::bad_any_cast & e ) {
                    return false;
                }
            };
            
            template <class T> void set( const std::string & key, const T & value ) {
                __attributes[key] = value;
            };
            
            bool was_activated() {
                return _activated;
            };
            
        protected:
            
            bool _activated;
            
        private:

            frame_attributes_t __attributes;
            
    };
    
}; // namespace xenon
