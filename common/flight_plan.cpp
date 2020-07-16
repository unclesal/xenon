// *********************************************************************************************************************
// *                                              Реализация полетного плана                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 09 jul 2020 at 08:42 *
// *********************************************************************************************************************
#include "flight_plan.h"
#include "utils.hpp"

using namespace xenon;

#ifdef INSIDE_AGENT
uint16_t FlightPlan::__NPP = 0;
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Пустой конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

FlightPlan::FlightPlan() {
    __flight_number = "";
    __departure = "";
    __destination = "";
    __alternate = std::vector<std::string>();
    __cruise_altitude = 0.0;
    __way = std::deque<waypoint_t>();

#ifdef INSIDE_AGENT
    __agent = nullptr;
#endif    
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Конструктор с параметрами                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

FlightPlan::FlightPlan(
    const std::string & flight_number,
    const std::string & departure,
    const std::string & destination,
    const std::vector <std::string> & alternate,
    // Высота - в футах
    const float & cruise_altitude
) {
    
    __flight_number = flight_number;
    __departure = departure;
    __destination = destination;
    __alternate = alternate;
    __cruise_altitude = cruise_altitude;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Оператор присваивания                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

FlightPlan & FlightPlan::operator = ( const FlightPlan & fp ) {
    
    __flight_number = fp.__flight_number;
    __departure = fp.__departure;
    __destination = fp.__destination;
    __alternate = fp.__alternate;
    __cruise_altitude = fp.__cruise_altitude;
    __way = fp.__way;
    return * this;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Конструктор копирования                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

FlightPlan::FlightPlan( const FlightPlan & fp ) {
    * this = fp;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Увеличение счетчика номера точек полетного плана                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_AGENT
void FlightPlan::__increment_npp() {
    FlightPlan::__NPP ++;
    if ( FlightPlan::__NPP >= (uint16_t) 0xfffffff0 ) FlightPlan::__NPP = 0;
};
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Добавить точку в начало                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void FlightPlan::push_front( waypoint_t & wp ) {
    
    __mutex.lock();
#ifdef INSIDE_AGENT    
    // Присваиваем номер данной точки.
    wp.npp = FlightPlan::__NPP;
    FlightPlan::__increment_npp();
#endif    
    __way.push_front( wp );
    __mutex.unlock();
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Удалить точку из начала полетного плана                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void FlightPlan::pop_front() {
    
    __mutex.lock();

#ifdef INSIDE_AGENT
    waypoint_t front_wp = __way.at(0);
#endif    

    __way.pop_front();
    __mutex.unlock();
    
#ifdef INSIDE_AGENT
    if ( __agent ) __agent->wp_reached( front_wp );    
#endif    

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Удалить от начала полетного плана все точки вплоть до данного номера по порядку                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void FlightPlan::erase_up_to( const uint16_t & npp) {
    
    __mutex.lock();
    
    deque<waypoint_t>::iterator it = __way.begin();
    while ( it != __way.end() ) {
        if (( * it ).npp == npp ) {
            // Точка найдена.
            __way.erase( __way.begin(), it );
            break;
        };
        it ++;
    };
    
    __mutex.unlock();
    
}



// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Добавить точку в конец                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void FlightPlan::push_back( waypoint_t & wp ) {
    
    __mutex.lock();

#ifdef INSIDE_AGENT    
    wp.npp = FlightPlan::__NPP;
    FlightPlan::__increment_npp();
#endif
    
    __way.push_back( wp );
    __mutex.unlock();
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Вставить точку                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void FlightPlan::insert( const int & index, waypoint_t & wp ) {
    __mutex.lock();
    
#ifdef INSIDE_AGENT
    wp.npp = FlightPlan::__NPP;
    FlightPlan::__increment_npp();
#endif    
    
    deque<waypoint_t>::iterator it;
    
    if ( index >= 0 ) it = __way.begin() + index; 
    else it = __way.end() + index; // Сам индекс - отрицательный при этом, поэтому плюс.
    
    __way.insert( it, wp );
    
    __mutex.unlock();
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Перерасчет дистанций и курсов                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void FlightPlan::recalculate() {
    
    for ( int i=0; i < (int) __way.size() - 1; ++i ) {
        waypoint_t & at_i = __way.at( i );
        waypoint_t & at_n = __way.at( i + 1 );
        at_i.distance_to_next_wp = xenon::distance2d(at_i.location, at_n.location );
        auto bearing = xenon::bearing( at_i.location, at_n.location );
        at_i.outgoing_heading = bearing;
        at_n.incomming_heading = bearing;
    };

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Удалить точку по указанному индексу                                         *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void FlightPlan::remove( const int & index ) {
    __mutex.lock();
    
    if ( __way.size() > index ) {
        __way.erase( index );
    } else {
        Logger::log(
            "FlightPlan::remove " + std::to_string(index) + ", but FP size=" + std::to_string( __way.size() )
        );
    };
    __mutex.unlock();
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Получить копию точки по указанному индексу                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

xenon::waypoint_t FlightPlan::get( const int & index ) {
    
    static waypoint_t fake;
    waypoint_t result = fake;
    
    __mutex.lock();
    
    if ( (int) __way.size() > index ) result = __way.at( index );
    else Logger::log(
        "FlightPlan::get " + std::to_string( index ) + ", but FP size=" + std::to_string( __way.size() )
    );
        
    __mutex.unlock();
    
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                   Заменить точку. При этом точка должна уже существовать, это не вставка.                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void FlightPlan::set( const int & index, const xenon::waypoint_t & wp) {
    
    __mutex.lock();
    if ( __way.size() > index ) __way.at( index ) = wp;
    else Logger::log(
        "FlightPlan::set " + std::to_string( index ) + ", but FP size=" + std::to_string( __way.size() )
    );
    __mutex.unlock();
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Очистка полетного плана                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void FlightPlan::clear() {
    
    __mutex.lock();
    __way.clear();
    __mutex.unlock();
    
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Выдать размер полетного плана                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

unsigned int xenon::FlightPlan::size() {
    
    unsigned int result = 0;
    __mutex.lock();
    result = __way.size();
    __mutex.unlock();
    return result;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Пустой он или нет?                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

bool FlightPlan::is_empty() {
    
    bool result = false;
    __mutex.lock();
    result = __way.empty();
    __mutex.unlock();
    return result;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Расстояние до ВПП (до точки, которая имеет тип WAYPOINT_RUNWAY)                            *
// *                                                                                                                   *
// *********************************************************************************************************************

double FlightPlan::distance_to_runway( const location_t & from ) {
                
    double result = 0.0;
    
    __mutex.lock();
    
    for ( int i=0; i<__way.size(); ++i ) {
        auto wp = __way.at(i);
        if ( wp.type == WAYPOINT_RUNWAY ) {
            result = xenon::distance2d( from, wp.location );
            break;
        }
    };
    
    __mutex.unlock();
    
    return result;
    
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Расстояние до "достаточно крутого поворота"                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

double FlightPlan::distance_to_turn( const location_t & from, waypoint_t & turned_wp ) {
    
    double result = 0.0;
    
    __mutex.lock();
    
    for ( int i=0; i < ( (int) __way.size() ); ++i ) {
        auto wp = __way.at(i);
        double change = wp.incomming_heading - wp.outgoing_heading;
        if ( abs(change) >= 20.0 ) {
            turned_wp = wp;
            result = xenon::distance2d( from, wp.location );
            break;
        }
    }
    
    __mutex.unlock();
    
    return result;
    
};
