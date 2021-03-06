// *********************************************************************************************************************
// *                                              Реализация полетного плана                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 09 jul 2020 at 08:36 *
// *********************************************************************************************************************
#pragma once

#include <deque>
#include <string>
#include <mutex>

#include "structures.h"
#include "logger.h"

#ifdef INSIDE_AGENT
#include "agent_interface.h"
#endif


namespace xenon {
    
    class FlightPlan {
        
        friend class CmdFlightPlan;
        
        public:
            
            FlightPlan();
            FlightPlan(
                const std::string & flight_number,
                const std::string & departure,
                const std::string & destination,
                const vector <std::string> & alternate,
                // Высота - в футах
                const float & cruise_altitude
            );
            FlightPlan( const FlightPlan & fp );
            
            ~FlightPlan() = default;
            FlightPlan & operator = ( const FlightPlan & fp );
            
            void pop_front();
            
            /**
             * @short Удалить от начала полетного плана все точки вплоть до данной.
             * Отрабатывается при достижении определенной точки полетного плана в агенте.
             */
            void erase_up_to( const uint16_t & npp );
            
            void push_front( waypoint_t & wp );            
            void push_back( waypoint_t & wp ); 
            /**
             * @short Вставить точку.
             * Если индекс отрицательный, то вставляется от конца. Если положительный - то от начала.
             */
            void insert( const int & index, waypoint_t & wp );
            
            // void remove( const int & index );            
            
            waypoint_t get( const int & index );
            void set( const int & index, const waypoint_t & wp );
            void clear();
            
            unsigned int size();
            bool is_empty();
            void recalculate();
            
            double distance_to_runway( const location_t & from );
            double distance_to_turn( const location_t & from, waypoint_t & turned_wp );
            
            const std::string & flight_number() { return __flight_number; };
            const std::string & departure() { return __departure; };
            const std::string & destination() { return __destination; };
            const std::vector< std::string > & alternate() { return __alternate; };
            const float & cruise_altitude() { return __cruise_altitude; };
            
            void set_flight_number( const std::string & fn ) { __flight_number = fn; };
            void set_departure( const std::string & dep ) { __departure = dep; };
            void set_destination( const std::string & des ) { __destination = des; };
            void set_cruise_altitude( const float & a ) { __cruise_altitude = a; };
            void set_alternate( const vector<std::string> & alt ) { __alternate = alt; };
            
#ifdef INSIDE_AGENT
            void set_agent(AgentInterface * a ) {
                __agent = a;
            };
#endif                        

        protected:
            
        private:
            
            std::string __flight_number;
            std::string __departure;
            std::string __destination;
            std::vector <std::string> __alternate;
            float __cruise_altitude;
            
            std::mutex __mutex;
            std::deque < waypoint_t > __way;

#ifdef INSIDE_AGENT
            AgentInterface * __agent;
            // Номер точки по порядку. Генерируется в агенте, остальные только читают его.
            static uint16_t __NPP;
            static void __increment_npp();
#endif            
        
    }; // class FlightPlan
    
}; // namespace xenon
