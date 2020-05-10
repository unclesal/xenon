// *********************************************************************************************************************
// *                                               Общие мелкие утилитки.                                              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:32 *
// *********************************************************************************************************************
#pragma once
#include <string>
#include <vector>

using namespace std;
using namespace xenon;

vector<string> split (const string & s, char delim);

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
}

/**
 * @short Перевод из градусов в радианы.
 * @param degrees
 * @return
 */

inline double degrees_to_radians( const double & degrees ) {
    return degrees * PI / 180.0 ;
}

