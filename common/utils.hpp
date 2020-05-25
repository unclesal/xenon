// *********************************************************************************************************************
// *                                               Общие мелкие утилитки.                                              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:32 *
// *********************************************************************************************************************
#pragma once
#include <string>
#include <vector>

#include "constants.h"
#include "structures.h"

using namespace std;
using namespace xenon;

namespace xenon {

    vector< string > split( const string & s, char delim );

    /**
     * @short Нормализация градусов.
     * Курс (азимут) может быть в пределах 0-360.
     */

    inline void normalize_degrees( double & degrees ) {

        while ( degrees > 360.0 ) degrees -= 360.0;
        while ( degrees < 0.0 ) degrees += 360.0;

    }

    /**
     * @short Перевод из радиан в градусы.
     * @param radians
     * @return
     */

    inline double radians_to_degrees( const double & radians ) {
        double degrees = radians * 180.0 / PI;
        normalize_degrees( degrees );
        return degrees;
    };

    /**
     * @short Перевод из градусов в радианы.
     * @param degrees
     * @return
     */

    inline double degrees_to_radians( const double & degrees ) {
        return degrees * PI / 180.0;
    };
    
    inline double meters_per_second_to_knots( const double & mps ) {
        // 1 м/с = 1.9438444924574 kt
        // return 3600.0 * mps / (double) M_per_NM;
        return mps * 1.9438444924574;            
    };

    inline double knots_to_merets_per_second( const double & kt ) {
        // 1 kt = 0.51444444444 м/с
        return 0.51444444444 * kt;
    };
    
    inline double meters_per_seconds_to_feet_per_min( const double mps ) {
        // 1 фут в минуту = 0.00507999983744 метра в секунду.
        return mps / 0.00507999983744;
    };
    
    inline double feet_per_min_to_meters_per_second( const double fpm ) {
        return fpm * 0.00507999983744;
    };

    /**
     * @short Полная дистанция в сферических координатах.
     */

    static inline double distance( const location_t & from, const location_t & to ) {
        auto boost_from = boost_location_t( from.latitude, from.longitude, from.altitude );
        auto boost_to = boost_location_t( to.latitude, to.longitude, to.altitude );
        double distance = boost::geometry::distance( boost_from, boost_to, geoid_distance_t());
        return distance;
    };
    
    /**
     * @short Курс (азимут) от точки на точку.
     */

    static inline double bearing(const location_t & location_from, const location_t & location_to ) {
        double teta1 = degrees_to_radians( location_from.latitude ); // lat
        double teta2 = degrees_to_radians( location_to.latitude ); // lat2
        double delta1 = degrees_to_radians( location_to.latitude - location_from.latitude ); // lat2-lat
        double delta2 = degrees_to_radians( location_to.longitude - location_from.longitude ); // lon2-lon

        //==================Heading Formula Calculation================//

        double y = sin(delta2) * cos(teta2);
        double x = cos(teta1)*sin(teta2) - sin(teta1)*cos(teta2)*cos(delta2);
        double brng = atan2(y,x);
        brng = radians_to_degrees(brng);// radians to degrees
        normalize_degrees( brng );
        return brng;
    };
    
    /**
     * @short Преобразование градусов из минут-секунд - в десятичную форму.
     */
    
    inline double degrees_to_decimal( const double & grad, const double & min, const double & sec, const char & dir ) {
        double result = grad + min / 60.0 + sec / 3600.0;
        if ( ( dir == 'S' ) || ( dir == 'W' ) ) result = -result;
        return result;
    };


}; // namespace xenon
