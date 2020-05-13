// *********************************************************************************************************************
// *                                                    Аэропорт.                                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 13:22 *
// *********************************************************************************************************************

#include <string>
#include <clocale>
#include <iostream>
#include <fstream>
#include <filesystem>

#ifdef INTERNAL_XPLANE
#include "XPLMNavigation.h"
#endif

#include "structures.h"
#include "utils.hpp"
#include "airport.h"
#include "exceptions.h"
#include "airport_network.h"

namespace fs = std::filesystem;
using namespace std;
using namespace lemon;
using namespace xenon;

// Статические найденные аэропорты.
map<string, Airport> Airport::_airports_;
bool Airport::_airports_was_readed_ = false;
// Обработчик события чтения нового аэропорта.
void ( * Airport::_has_been_parsed_ )( string & icao_code ) = nullptr;
// Фейковый аэропорт.
Airport Airport::_fake_airport_;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Пустой конструктор.                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

Airport::Airport() {
    _full_path_to_apt_dat_ = "";
    _origin_ = "";
    _version_ = 0;
    _ap_type_ = AP_TYPE_UNKNOWN;
    _evalution_in_feet_ = 0;
    _icao_code_ = "";
    _name_ = "";
    _viewpoint_ = viewpoint_t();

    // Атрибуты аэропорта.
    _city_ = "";
    _country_ = "";
    _datum_lon_ = 0.0;
    _datum_lat_ = 0.0;
    _gui_label_ = "";
    _iata_code_ = "";
    _region_code_ = "";
    _state_ = "";
    _transition_alt_ = "";
    _transition_level_ = "";
    _faa_code_ = "";
    _flatten_ = 0;
    _drive_on_left_ = false;
    _local_code_ = "";

    // Объекты внутри аэропорта.

    _land_runways = vector< land_runway_t >();
    _water_runways = vector< water_runway_t >();
    _helipad_runways = vector< helipad_runway_t >();
    _startup_locations_ = map< string, startup_location_t >();
    _truck_parkings_ = vector< truck_parking_t >();
    _truck_destinations_ = vector< truck_destination_t >();

    // Объекты-контейнеры нодов внутри аэропорта.
    _light_beacons_ = vector< light_beacon_t >();
    _frequencies_ = vector< frequency_t >();
    _windsocks_ = vector< windsock_t >();
    _signs_ = vector< sign_t >();
    _lighting_objects_ = vector< lighting_objects_t >();
    _taxiways_ = vector< taxiway_t >();
    _linear_features_ = vector< linear_feature_t >();
    _boundaries_ = vector< boundary_t >();

    // traffic rules flow
    _traffic_flow_ = vector< traffic_flow_t >();
    _taxi_network = taxi_network_t();

    // "Осиротевшесть" аэропорта.
    _orphanded = true;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Оператор присвоения                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
Airport & Airport::operator = (const Airport & apt) {

    _full_path_to_apt_dat_ = apt._full_path_to_apt_dat_;
    _origin_ = apt._origin_;
    _version_ = apt._version_;
    _ap_type_ = apt._ap_type_;
    _evalution_in_feet_ = apt._evalution_in_feet_;
    _icao_code_ = apt._icao_code_;
    _name_ = apt._name_;
    _viewpoint_ = apt._viewpoint_;

    // Атрибуты аэропорта.
    _city_ = apt._city_;
    _country_ = apt._country_;
    _datum_lon_ = apt._datum_lon_;
    _datum_lat_ = apt._datum_lat_;
    _gui_label_ = apt._gui_label_;
    _iata_code_ = apt._iata_code_;
    _region_code_ = apt._region_code_;
    _state_ = apt._state_;
    _transition_alt_ = apt._transition_alt_;
    _transition_level_ = apt._transition_level_;
    _faa_code_ = apt._faa_code_;
    _flatten_ = apt._flatten_;
    _drive_on_left_ = apt._drive_on_left_;
    _local_code_ = apt._local_code_;

    // Объекты внутри аэропорта.
    _land_runways = apt._land_runways;
    _water_runways = apt._water_runways;
    _helipad_runways = apt._helipad_runways;
    _startup_locations_ = apt._startup_locations_;
    _truck_parkings_ = apt._truck_parkings_;
    _truck_destinations_ = apt._truck_destinations_;

    // Объекты-контейнеры нодов внутри аэропорта.
    _light_beacons_ = apt._light_beacons_;
    _frequencies_ = apt._frequencies_;
    _windsocks_ = apt._windsocks_;
    _signs_ = apt._signs_;
    _lighting_objects_ = apt._lighting_objects_;
    _taxiways_ = apt._taxiways_;
    _linear_features_ = apt._linear_features_;
    _boundaries_ = apt._boundaries_;

    _traffic_flow_ = apt._traffic_flow_;
    _taxi_network = apt._taxi_network;

    _orphanded = apt._orphanded;

    // Копирование графа.
    _routes = apt._routes;

    return * this;
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Конструктор копирования                                               *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
Airport::Airport( const Airport & apt )
        : Airport()
{
    * this = apt;
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Парзим - один файл.                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::read( const string & full_path_to_apt_dat ) {

    if ( ! std::filesystem::exists(full_path_to_apt_dat)) return;

    string origin;
    int version = 0;
    // Динамически создаваемые указатели на элементы, которые могут иметь продолжение в файле.
    // А могут и не иметь. Поэтому наличие в них какого-то значения надо учитывать при выходе из процедуры.
    Airport * apt = nullptr;
    node_container_t * node_container = nullptr;
    startup_location_t * startup_location = nullptr;
    traffic_flow_t * traffic_flow = nullptr;

    ifstream file( full_path_to_apt_dat );

//    std::locale locale("C");
//    file.imbue(locale);
//
//    char point = std::use_facet< std::numpunct<char> >(file.getloc()).decimal_point();
//    XPlaneUtilities::log( string("Delimiter is: ") + point );

    int lines_count = 0;
    for( std::string line; getline( file, line ); ) {
        // Счетчик числа строк, для отладки, если в файле будут найдены
        // ошибки или данный файл будет некорректно обработан.
        lines_count ++;

//        if (
//            (full_path_to_apt_dat == "/home/sal/program/X-Plane 11/Custom Scenery/RescueX_Lib/Earth nav data/apt.dat")
//            && (lines_count == 87)
//        ) {
//            cout << full_path_to_apt_dat << ", " << lines_count << endl;
//        }

        // Удаление из строки символов возврата каретки и перевода строки.
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());

        // Если до сих пор не заполнен Origin, то там допускается только два варианта.
        if ( origin.empty() ) {
            if (( line == "I" ) || ( line == "A" )) {
                origin = line;
                continue;
            } else throw bad_format_exception( string("Bad origin line ") + line );
        }

        // Разбиваем строку на подстроки, разделителем является пробел.
        vector<string> contents = split(line, ' ');
        // Удаляем пустые элементы полученного массива.
        contents.erase(
            remove_if(contents.begin(), contents.end(), [](string const & s) { return s.empty(); }),
            contents.end()
        );

        if ( contents.empty() ) continue;
        // В некоторых попадается решетка перед типом.
        int pos = contents[0].find('#');
        if ( pos >= 0 ) contents[0].erase(pos, 1);
        int i_type = stoi(contents.at(0));

        // Если до сих пор нет версии - то пытаемся ее заполнить.

        if ( ! version ) {
            if ( i_type >= 850 ) { // Меньше я ее вроде не встречал.
                version = i_type;
                continue;
            }
        }

//        if (
//            (apt) && ( apt->_icao_code_ == "USSS" ) && ( i_type == 1300 ) && (line.find(" 13") > 0)
//            && ( line.find("heavy") >0 )
//        ) {
//            cout << "Here";
//        };

        // -------------------------------------------------------------------------------------------------------------
        //
        //                                   Все. Дальше пошли элементы аэропорта.
        //
        // -------------------------------------------------------------------------------------------------------------

        if (( i_type == AP_TYPE_LAND ) || ( i_type == AP_TYPE_SEA ) || ( i_type == AP_TYPE_HELI )) {
            // Начало аэропорта.
            if ( apt ) {
                // Если у нас уже что-то было насчитано, то аэропорт надо поместить в массив результата.
                // Потому что начался следующий. При этом если элемент уже есть, то надо попытаться
                // сохранить его атрибуты (1302 которые).
                if ( startup_location ) apt->_put_startup_location( & startup_location );
                if ( node_container ) apt->_put_node_container( & node_container );
                if ( traffic_flow ) apt->_put_traffic_flow( & traffic_flow );
                Airport::_put_airport( & apt );
            }
            apt = new Airport();
            // Начальное заполнение того, что уже должно быть - элементы
            // одни и те же для всех аэропортов данного файла (поскольку
            // в одном файле может быть - несколько аэропортов).
            apt->_origin_ = origin;
            apt->_version_ = version;
            apt->_full_path_to_apt_dat_ = full_path_to_apt_dat;
            // Здесь же - элементы этой самой начальной строки.
            apt->_ap_type_ = i_type;
            apt->_evalution_in_feet_ = stoi(contents.at(1));
            apt->_icao_code_ = contents.at(4);
            pos = line.find( apt->_icao_code_ );
            pos += apt->_icao_code_.length();
            apt->_name_ = line.substr(pos);
            while (apt->_name_.at(0) == ' ') apt->_name_.erase( 0, 1 );
            continue;
        }

        if ( i_type == 14 ) {
            // viewpoint
            if ( ! apt ) throw runtime_error("got viewpoint while apt is none. Line=" + to_string(lines_count));
            viewpoint_t vp;
            vp.latitude = stod( contents[1] );
            vp.longitude = stod( contents[2] );
            vp.height = stoi( contents[3] );
            // 4 - not used yet.
            // С 5ой позиции начиная и там их может быть много с пробелами.
            pos = line.find(contents[5]);
            vp.name = line.substr( pos );
            apt->_viewpoint_ = vp;
            continue;
        }

        if ( i_type == 15 ) {
            // startup location - видимо, в старом стиле, дублирует 1300.
            if ( ! apt ) throw runtime_error(
                    "got startup location but apt is empty. line=" + to_string(lines_count)
                );
            if ( startup_location ) apt->_put_startup_location( & startup_location );
            startup_location = new startup_location_t();
            startup_location->latitude = stod( contents[1] );
            startup_location->longitude = stod( contents[2] );
            startup_location->heading = stof( contents[3] );
            // Имя - это вся строка до конца.
            pos = line.rfind( contents[4] );
            startup_location->name = line.substr( pos );

//            if ( apt->_icao_code_ == "USSS") {
//                XPlaneUtilities::log(
//                    "file=" + full_path_to_apt_dat
//                    + ", line=" + line
//                    + ", got old ramp " + startup_location->name
//                    + ", lat=" + to_string( startup_location->latitude)
//                    + ", lon=" + to_string( startup_location->longitude)
//                );
//            }

            continue;
        }

        if ( i_type == 18 ) {
            // Light beacon
            if ( ! apt ) throw runtime_error("got light beacon while apt is empty, line=" + to_string(lines_count));
            light_beacon_t lb;
            lb.latitude = stod( contents[1] );
            lb.longitude = stod( contents[2] );
            lb.type_code = stoi( contents[3] );
            if ( contents.size() >= 5 ) {
                pos = line.find( contents[4] );
                lb.name = line.substr( pos );
            }
            apt->_light_beacons_.push_back(lb);
            continue;
        }

        if ( i_type == 19 ) {
            // windsock.
            if ( ! apt ) throw runtime_error("Got a windsock while apt is empty, line=" + to_string(lines_count));
            windsock_t ws;
            ws.latitude = stod( contents[1] );
            ws.longitude = stod( contents[2] );
            ws.flag_lighting = stoi( contents[3] );
            if ( contents.size() >= 5 ) {
                pos = line.find(contents[4]);
                ws.name = line.substr(pos);
            }
            apt->_windsocks_.push_back( ws );
            continue;
        }

        if ( i_type == 20 ) {
            // Sign
            if ( ! apt ) throw runtime_error("got sign while apt is none. Line=" + to_string(lines_count));
            sign_t sign;
            sign.latitude = stod( contents[1] );
            sign.longitude = stod( contents[2] );
            sign.orientation = stof( contents[3] );
            // 4 - в данный момент не используется.
            sign.size_code = stoi( contents[5] );
            sign.label = contents[6];
            apt->_signs_.push_back(sign);
            continue;
        }

        if ( i_type == 21 ) {
            // Lighting object.
            if ( ! apt ) throw runtime_error("got lighting object, apt is none. line=" + to_string(lines_count));
            lighting_objects_t lo;
            lo.latitude = stod( contents[1] );
            lo.longitude = stod( contents[2] );
            lo.object_type = stoi( contents[3] );
            lo.orientation = stof( contents[4] );
            lo.glidescope_angle = stof( contents[5] );
            lo.runway_number = contents[6];
            // description is optional.
            if ( contents.size() >= 8 ) lo.description = contents[7];
            apt->_lighting_objects_.push_back( lo );
            continue;
        }

        if (
            ( i_type >= FREQ_OLD_ATC_RECORDED && i_type <= FREQ_OLD_ATC_DEP )
            || ( i_type >= FREQ_ATC_RECORDED && i_type <= FREQ_ATC_DEP )
        ) {
            // Частоты. Коды начинаются от 50.
            if ( ! apt ) throw runtime_error("Got frequency but apt is empty, line=" + to_string(lines_count) );
            frequency_t freq;
            freq.freq_type = i_type;
            freq.frequency = stoi( contents[1] );
            if ( contents.size() >= 3 ) {
                pos = line.find(contents[2]);
                freq.description = line.substr(pos);
            }
            apt->_frequencies_.push_back( freq );
            continue;
        }

        if ( i_type == 100 ) {
            // LAND RUNWAYs.
            if ( ! apt ) throw runtime_error(
                "got landing RWY while apt is none, line=" + to_string(lines_count)
            );

            land_runway_t rwy;
            rwy.width = stof( contents[1] );
            rwy.surface_type = stoi( contents[2] );
            rwy.shoulder_surface_type = stoi( contents[3] );
            rwy.smoothness = stof( contents[4] );
            rwy.centre_line_light = stoi( contents[5] );
            rwy.edge_light = stoi( contents[6] );
            rwy.distance_remaining_sign = stoi( contents[7] );
            // Дальше атрибутов может уже не быть.
            if ( contents.size() >= 9 ) rwy.runway_number = contents[8];
            if ( contents.size() >= 10 ) rwy.end_latitude = stod( contents[9] );
            if ( contents.size() >= 11 ) rwy.end_longitude = stod( contents[10] );
            if ( contents.size() >= 12 ) rwy.displaced_threshold_length = stof( contents[11] );
            if ( contents.size() >= 13 ) rwy.overrun_length = stof( contents[12] );
            if ( contents.size() >= 14 ) rwy.markings = stoi( contents[13] );
            if ( contents.size() >= 15 ) rwy.approach_ligth = stoi( contents[14] );
            if ( contents.size() >= 16 ) rwy.tdz_ligth = stoi( contents[15] );
            if ( contents.size() >= 17 ) rwy.reil_light = stoi( contents[16] );
            apt->_land_runways.push_back( rwy );
            continue;
        }

        if ( i_type == 101 ) {
            // WATER RUNWAYs.
            if ( ! apt ) throw runtime_error(
                "got water RWY while apt is none, line=" + to_string(lines_count)
            );

            water_runway_t rwy;
            rwy.width = stof( contents[1] );
            rwy.perimeter_buoys = stoi( contents[2] );
            // Дальше атрибутов может уже не быть.
            if ( contents.size() >= 4 ) rwy.runway_number = contents[3];
            if ( contents.size() >= 5 ) rwy.latitude = stod( contents[4] );
            if ( contents.size() >= 6 ) rwy.longitude = stod( contents[5] );
            apt->_water_runways.push_back( rwy );
            continue;
        }

        if ( i_type == 102 ) {
            // HELIPAD RUNWAYs
            if ( ! apt ) throw runtime_error(
                "got helipad RWY while apt is none, line=" + to_string( lines_count )
            );

            helipad_runway_t rwy;
            rwy.designator = contents[1];
            rwy.latitude = stod( contents[2] );
            rwy.longitude = stod( contents[3] );
            rwy.orientation = stof( contents[4] );
            rwy.length = stof( contents[5] );
            rwy.width = stof( contents[6] );
            rwy.surface_code = stoi( contents[7] );
            rwy.markings = stoi( contents[8] );
            rwy.surface_type = stoi( contents[9] );
            if (contents.size() >= 11 ) {
                rwy.smoothness = stof( contents[ 10 ] );
                if ( contents.size() >= 12 ) rwy.edge_light = stoi( contents[ 11 ] );
            }
            apt->_helipad_runways.push_back( rwy );
            continue;
        }

        if ( i_type == CONTAINER_TAXIWAY ) { // 110
            // pavement (taxiway).
            if ( ! apt ) throw runtime_error(
                "got taxiway while apt is none, line=" + to_string( lines_count )
            );
            if ( node_container ) apt->_put_node_container( & node_container );
            node_container = new taxiway_t();
            auto * twy = ( taxiway_t * ) node_container;
            twy->container_type = CONTAINER_TAXIWAY;
            twy->surface_type = stoi( contents[1] );
            twy->smoothness = stof( contents[2] );
            twy->orientation = stof( contents[3] );
            if ( contents.size() >= 5 ) {
                pos = line.find(contents[4]);
                twy->description = line.substr( pos );
            }

            continue;
        }

        if ( i_type == CONTAINER_LINEAR_FEATURE ) {
            if ( ! apt ) throw runtime_error("got linear feature while apt is none, line=" + to_string(lines_count));
            if ( node_container ) apt->_put_node_container( & node_container );
            node_container = new linear_feature_t();
            auto * lft = ( linear_feature_t * ) node_container;
            lft->container_type = CONTAINER_LINEAR_FEATURE;
            lft->name = line.substr(3);
            // Удаление начальных пробелов.
            while (lft->name[0] == ' ') lft->name.erase(0, 1);
            continue;
        }

        if ( i_type == CONTAINER_BOUNDARY ) {
            if ( ! apt ) throw runtime_error(
                "got boundary element when apt is none, line=" + to_string( lines_count )
            );
            if ( node_container ) apt->_put_node_container( & node_container );
            node_container = new boundary_t();
            auto * bdy = ( boundary_t * ) node_container;
            bdy->container_type = CONTAINER_BOUNDARY;
            bdy->description = contents[1];
            continue;
        }

        if ( ( i_type == NODE_PLAIN ) || ( i_type == NODE_BEZIER )
            || ( i_type == NODE_CLOSE_BOUNDARY) || ( i_type == NODE_CLOSE_BEZIER )
            || ( i_type == NODE_END_LINE ) || ( i_type == NODE_END_BEZIER )
        ) {
            // Нода.
            if ( ! apt ) throw runtime_error(
                "got node while apt is empty, line " + to_string( lines_count )
            );
            if ( ! node_container ) throw runtime_error(
                "got node while container is empty, line " + to_string( lines_count )
            );
            node_t node;
            node.node_type = i_type;
            node.latitude = stod( contents[1] );
            node.longitude = stod( contents[2] );
            int next_elem = 3;
            if (( i_type == NODE_BEZIER ) || ( i_type == NODE_CLOSE_BEZIER ) || ( i_type == NODE_END_BEZIER )) {
                node.bezier_point_latitude = stod( contents[ next_elem ++ ] );
                node.bezier_point_longitude = stod( contents[ next_elem ++ ] );
            }
            if ( contents.size() >= next_elem + 2) {
                node.line_type_code = stoi( contents[ next_elem ++ ] );
                node.lighting_type_code = stoi( contents[next_elem] );
            }
            node_container->nodes.push_back(node);
            continue;
        }

        if ( i_type == 1000 ) {
            // traffic_flow
            if ( ! apt ) throw runtime_error("Got traffic flow while apt is empty, line=" + to_string( lines_count ));
            if ( traffic_flow ) apt->_put_traffic_flow( & traffic_flow );
            traffic_flow = new traffic_flow_t();
            pos = line.find( contents[1] );
            traffic_flow->name = line.substr( pos );
            continue;
        }

        if ( i_type == 1001 ) {
            // Traffic flow wind rule
            if ( ! apt ) throw runtime_error("Got rule wind while apt is empty, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got rule wind while traffic flow is empty, line=" + to_string( lines_count )
            );
            traffic_flow_wind_rule_t tw;
            tw.metar_station = contents[1];
            tw.wind_direction_minimum = stoi( contents[2] );
            tw.wind_direction_maximum = stoi( contents[3] );
            tw.wind_speed_maximum = stoi( contents[4] );
            traffic_flow->wind_rules.push_back( tw );
            continue;
        }

        if ( i_type == 1002 ) {
            // Traffic flow ceiling rule
            if ( ! apt ) throw runtime_error(
                "Got fraffic ceiling rule while apt is empty, line=" + to_string( lines_count )
            );
            if ( ! traffic_flow ) throw runtime_error(
                "Got ceiling rule while traffic flow is empty, line=" + to_string( lines_count )
            );
            traffic_flow_ceiling_rule_t cr;
            cr.metar_station = contents[1];
            cr.minimum_height = stoi( contents[2] );
            traffic_flow->ceiling_rules.push_back( cr );
            continue;
        }

        if ( i_type == 1003 ) {
            // Traffic flow visibility rule
            if ( ! apt ) throw runtime_error("Got visibility rule but apt is emtpy, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got visibility rule while traffic_flow is emtpy, line=" + to_string( lines_count )
            );
            traffic_flow_visibility_rule_t vis;
            vis.metar_station = contents[1];
            vis.minimum_visibility = stof( contents[2] );
            traffic_flow->visibility_rules.push_back( vis );
            continue;
        }

        if ( i_type == 1004 ) {
            // Traffic time rule
            if ( ! apt ) throw runtime_error("Got time rule, apt is empty, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got time rule, traffic flow empty, line=" + to_string( lines_count )
            );

            // Эта ситуация - бывает. Пока не знаю, что с ней делать.
            // if (( ! traffic_flow->time_from.empty() ) || ( ! traffic_flow->time_to.empty() ))
            //    throw runtime_error("got traffic time rule with already timed, line=" + to_string( lines_count ));

            traffic_flow->time_from = contents[1];
            traffic_flow->time_to = contents[2];
            continue;
        }

        if ( i_type == 1100 || i_type == 1110 ) {
            // runway in use
            if ( ! apt ) throw runtime_error("Got runway in use, but apt is empty, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got runway in use, but traffic flow is empty, line=" + to_string( lines_count )
            );
            traffic_flow_runway_in_use_t riu;
            riu.runway = contents[1];
            riu.frequency = stoi( contents[2] );
            riu.rule_type = contents[3];
            riu.airplane_types = contents[4];
            riu.on_course_heading = contents[5];
            riu.atc_heading = contents[6];
            if ( contents.size() >= 8 ) {
                pos = line.find( contents[ 7 ] );
                riu.name = line.substr( pos );
            }
            traffic_flow->runway_in_use.push_back( riu );
            continue;
        }

        if ( i_type == 1101 ) {
            // VFR pattern rule
            if ( ! apt ) throw runtime_error("Got VFR pattern but apt is empty, line=" + to_string( lines_count ));
            if ( ! traffic_flow ) throw runtime_error(
                "Got VFR pattern but traffic flow is empty, line=" + to_string(lines_count)
            );
            string rwy = contents[1];
            string direction = contents[2];
            traffic_flow->vfr_pattern[rwy] = direction;
            continue;
        }
        if ( i_type == 1200 ) {
            // taxi network
            if ( ! apt ) throw runtime_error("got taxi network while apt is empty, line=" + to_string( lines_count ));
            continue;
        }

        if ( i_type == 1201 ) {
            // taxi routing NODE.
            if ( ! apt ) throw runtime_error("got taxi routing node but apt is emtpy, line=" + to_string( lines_count ));

//            // Old style, x-plane structure.
//            taxi_network_routing_node_t old_style_node;
//            old_style_node.latitude = stod( contents[1] );
//            old_style_node.longitude = stod( contents[2] );
//            old_style_node.usage = contents[3];
//            old_style_node.id = stoi( contents[4] );
//            if (contents.size() >= 6 ) {
//                pos = line.find( contents[5] );
//                old_style_node.name = line.substr( pos );
//            }
//            apt->_taxi_network.nodes.push_back( old_style_node );

            // using airport network
            apt->__routes.add_apt_dat_node( line, contents );

            continue;
        }

        if (( i_type == 1202 ) || ( i_type == 1206 )) {
            // Taxi routing EDGE. Segment in taxi routing network
            if ( ! apt ) throw runtime_error("got routing edge but apt is empty, line=" + to_string( lines_count ));
            apt->__routes.add_apt_dat_edge( i_type, line, contents );

//            // old style, native x-plane graph.
//            taxi_network_routing_edge_t old_style_edge;
//            old_style_edge.start_id = stoi( contents[1] );
//            old_style_edge.end_id = stoi( contents[2] );
//            old_style_edge.directions = contents[3];
//            if ( contents.size() >= 5 ) {
//                // Он, оказывается, тоже не является обязательным.
//                old_style_edge.type = contents[4];
//                if ( contents.size() >= 6 ) {
//                    pos = line.rfind( contents[ 5 ] );
//                    old_style_edge.name = line.substr( pos );
//                }
//            }
//            if ( i_type == 1202 ) apt->_taxi_network.edges.push_back( old_style_edge );
//            else apt->_taxi_network.ground_vehicle_edges.push_back( old_style_edge );

            continue;
        }

        if ( i_type == 1204 ) {
            // Edge active zone Identifies an edge as in a runway active zone.
            if ( ! apt ) throw runtime_error("got active zone but apt is empty, line=" + to_string( lines_count ));

            apt->__routes.add_apt_dat_active_zone( line, contents );
//            // old style, x-plane based.
//            if ( apt->_taxi_network.edges.empty() ) throw runtime_error(
//                "got active zone but edges is empty, line=" + to_string( lines_count )
//            );
//
//            taxi_network_routing_edge_active_zone_t old_style_zone;
//            old_style_zone.classification = contents[1];
//            old_style_zone.runways = contents[2];
//            // Ставится на последний добавленный в taxi_network edge.
//            auto last_idx = apt->_taxi_network.edges.size() - 1;
//            apt->_taxi_network.edges[ last_idx ].active_zones.push_back( old_style_zone );
//            // old style, x-plane based.
//            if ( apt->_taxi_network.edges.empty() ) throw runtime_error(
//                "got active zone but edges is empty, line=" + to_string( lines_count )
//            );
//
//            taxi_network_routing_edge_active_zone_t old_style_zone;
//            old_style_zone.classification = contents[1];
//            old_style_zone.runways = contents[2];
//            // Ставится на последний добавленный в taxi_network edge.
//            auto last_idx = apt->_taxi_network.edges.size() - 1;
//            apt->_taxi_network.edges[ last_idx ].active_zones.push_back( old_style_zone );

            continue;
        }

        if ( i_type == 1300 ) {
            // startup location - видимо, в новом стиле, т.к. дублирует 15.
            if ( ! apt ) throw runtime_error(
                "got startup location but apt is empty. line=" + to_string(lines_count)
            );
            if ( startup_location ) apt->_put_startup_location( & startup_location );
            startup_location = new startup_location_t();
            startup_location->latitude = stod( contents[1] );
            startup_location->longitude = stod( contents[2] );
            startup_location->heading = stof( contents[3] );
            startup_location->startup_type = contents[4];
            startup_location->airplane_types = contents[5];
            // Наименование - вся строка до конца. Причем по последней
            // здесь ориентироваться плохо, она может быть короткой
            // и встречаться несколько раз в строке. Поэтому делаем - по предпоследней.
            if ( contents.size() >= 7 ) {
                pos = line.rfind( contents[ 6 ] );
                startup_location->name = line.substr( pos );
                while ( startup_location->name[ 0 ] == ' ' ) startup_location->name.erase( 0, 1 );
            }

//            if ( apt->_icao_code_ == "USSS") {
//                XPlaneUtilities::log(
//                    "file=" + full_path_to_apt_dat
//                    + ", line=" + line
//                    + ", got NEW ramp " + startup_location->name
//                    + ", lat=" + to_string( startup_location->latitude)
//                    + ", lon=" + to_string( startup_location->longitude)
//                );
//            }

            continue;
        }

        if ( i_type == 1301 ) {
            // ramp start metadata
            if ( ! apt ) throw runtime_error(
                "Got ramp start metadata but apt is empty. Line=" + to_string(lines_count)
            );
            if ( ! startup_location ) throw runtime_error(
                "Got ramp metadata but startup location is none, line=" + to_string(lines_count)
            );
            startup_location->width_code = contents[1];
            startup_location->operation_type = contents[2];
            if ( contents.size() >= 4 ) startup_location->airline = contents[3];
            continue;
        }

        if ( i_type == 1302 ) {
            // Airport identification metadata.
            if ( ! apt ) throw runtime_error(
                "got 1302 while apt is none, line=" + to_string( lines_count )
            );

            if ( contents.size() >= 3 ) {
                if ( contents[ 1 ] == "city" ) apt->_city_ = contents[ 2 ];
                else if ( contents[ 1 ] == "country" ) apt->_country_ = contents[ 2 ];
                else if ( contents[ 1 ] == "datum_lat" ) apt->_datum_lat_ = stod( contents[ 2 ] );
                else if ( contents[ 1 ] == "datum_lon" ) apt->_datum_lon_ = stod( contents[ 2 ] );
                else if ( contents[ 1 ] == "gui_label" ) apt->_gui_label_ = contents[ 2 ];
                else if ( contents[ 1 ] == "iata_code" ) apt->_iata_code_ = contents[ 2 ];
                else if ( contents[ 1 ] == "region_code" ) apt->_region_code_ = contents[ 2 ];
                else if ( contents[ 1 ] == "state" ) apt->_state_ = contents[ 2 ];
                else if ( contents[ 1 ] == "transition_alt" ) apt->_transition_alt_ = contents[ 2 ];
                else if ( contents[ 1 ] == "transition_level" ) apt->_transition_level_ = contents[ 2 ];
                else if ( contents[ 1 ] == "faa_code" ) apt->_faa_code_ = contents[ 2 ];
                else if ( contents[ 1 ] == "flatten" ) apt->_flatten_ = stoi( contents[ 2 ] );
                else if (( contents[ 1 ] == "drive_on_left" ) && ( contents[ 2 ].find( '1' ) >= 0 ))
                    apt->_drive_on_left_ = true;
                else if ( contents[ 1 ] == "local_code" ) apt->_local_code_ = contents[ 2 ];

                    // Они уже есть в основных свойствах, незачем вроде дублировать.
                else if (
                    ( contents[ 1 ] != "icao_code" ) && ( contents[ 1 ] != "airport_id" ) &&
                    ( contents[ 1 ] != "local_authority" )
                    )
                    XPlane::log( string( "Airport::read: unhandled metadata key " ) + contents[ 1 ] );
            }
            continue;
        }

        if ( i_type == 1400 ) {
            // truck parking.
            if ( ! apt ) throw runtime_error( "got truck parking but apt is empty, line=" + to_string( lines_count ));
            truck_parking_t tp;
            tp.latitude = stod( contents[1] );
            tp.longitude = stod( contents[2] );
            tp.heading = stof( contents[3] );
            tp.parking_type = contents[4];
            tp.baggage_train_type = stoi( contents[5] );
            if ( contents.size() >= 7 ) {
                pos = line.find( contents[6]);
                tp.name = line.substr( pos );
            }
            apt->_truck_parkings_.push_back( tp );
            continue;
        }

        if ( i_type == 1401 ) {
            // Truck destination.
            if ( ! apt ) throw runtime_error("Got truck destination but apt is none, line=" + to_string( lines_count ));

            truck_destination_t dest;
            dest.latitude = stod( contents[1] );
            dest.longitude = stod( contents[2] );
            dest.heading = stof( contents[3] );
            dest.truck_types = contents[4];
            if ( contents.size() >= 6 ) {
                pos = line.find( contents[5] );
                dest.name = line.substr( pos );
            }
            apt->_truck_destinations_.push_back( dest );

            continue;
        }

        if ( i_type != 99 ) { // EOF
            // Если здесь остались - элемент обработан не был.
            XPlane::log(
                string( "Airport::read(), unhandled type " ) + to_string( i_type )
                + ", file=" + full_path_to_apt_dat + ", line=" + to_string( lines_count )
            );
        }

    }
    // Вышли из цикла. Если кто-то из динамических
    // указателей остался живой, то надо его учесть.
    if ( apt ) {
        if ( startup_location ) apt->_put_startup_location( & startup_location );
        if ( node_container ) apt->_put_node_container( & node_container );
        if ( traffic_flow ) apt->_put_traffic_flow( & traffic_flow );
        // if ( taxi_network ) apt->_put_taxi_network( & taxi_network );
        // Сам аэропорт идет последним.
        Airport::_put_airport( & apt );
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Положить на место распарзенный аэропорт, освободить память                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::_put_airport(Airport **ptr_airport) {
    auto apt = * ptr_airport;
    string icao_code = apt->_icao_code_;

//    if ( icao_code == "USSS" ) {
//        cout << "USSS";
//    }

    if ( Airport::_airports_.contains( apt->_icao_code_ )) {
        // Замещение старого элемента аэропорта - более новым.
        Airport old = Airport::_airports_[ apt->_icao_code_ ];
        if ( apt->_city_.empty()) apt->_city_ = old._city_;
        if ( apt->_country_.empty() ) apt->_country_ = old._country_;
        if ( apt->_datum_lon_ == 0.0) apt->_datum_lon_ = old._datum_lon_;
        if ( apt->_datum_lat_ == 0.0 ) apt->_datum_lat_ = old._datum_lat_;
        if ( apt->_gui_label_.empty()) apt->_gui_label_ = old._gui_label_;
        if ( apt->_iata_code_.empty()) apt->_iata_code_ = old._iata_code_;
        if ( apt->_region_code_.empty()) apt->_region_code_ = old._region_code_;
        if ( apt->_state_.empty()) apt->_state_ = old._state_;
        if ( apt->_transition_alt_.empty() ) apt->_transition_alt_ = old._transition_alt_;
        if ( apt->_transition_level_.empty() ) apt->_transition_level_ = old._transition_level_;
        if ( apt->_faa_code_.empty()) apt->_faa_code_ = old._faa_code_;
        if ( apt->_flatten_ == 0 ) apt->_flatten_ = old._flatten_;
        if ( ! apt->_drive_on_left_ ) apt->_drive_on_left_ = old._drive_on_left_;
        if ( apt->_local_code_.empty() ) apt->_local_code_ = old._local_code_;
    }
    // "Полнота" ВПП заполняется в тот момент, когда аэропорт складывается в коллекцию.
    apt->_check_runway_fullness();
    Airport::_airports_[ apt->_icao_code_ ] = * apt;
    delete( apt );
    * ptr_airport = nullptr;
    // Если есть реактор на распарзенный аэропорт, то вызываем его.
    if ( Airport::_has_been_parsed_ ) _has_been_parsed_(icao_code);
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Проверка "полноты" ВПП аэропорта. При необходимости дополняется отсутствующими ВПП.                *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::_check_runway_fullness() {
    map<string, vector<taxi_network_routing_edge_t>> rwy_edges = get_runways_edges();
    for ( auto & pair : rwy_edges ) {
        // Имена содержат две взлетки через /
        string together_name = pair.first;
        vector <taxi_network_routing_edge_t> edges = pair.second;

        // Заполняем полностью коллекцию узлов для составной взлетки,
        // например 08L/26R. Узлы будут одни и те же, только направление
        // движения будет обратным.
        vector <taxi_network_routing_node_t> nodes = get_nodes_for( edges );

        // Распарзиваем имена. Их в норме должно быть два.
        vector<string> names = split(together_name, '/');
        // Все полученные "номера" (имена) ВПП должны находиться в коллекции ВПП.
        // Если их там нет, то просто ставим пока флаг. Потому что в процессе
        // данного цикла "разпознается" конец этой самой ВПП.
        string need_to_insert;
        taxi_network_routing_node_t nearest_founded_node;
        land_runway_t founded_runway;

        for ( const auto & runway_number : names ) {

            bool found = false;

            for ( const auto & rwy : _land_runways ) {
                if ( rwy.runway_number == runway_number ) {
                    // Взлетка - существует.
                    found = true;
                    founded_runway = rwy;
                    location_t location;
                    location.latitude = rwy.end_latitude;
                    location.longitude = rwy.end_longitude;
                    nearest_founded_node = get_nearest_node(location, nodes);
                    // Выход из цикла по имеющимся RWYs.
                    break;
                }
            }

            if ( ! found ) {
                // ВПП нет. Мы все еще находимся в цикле по именам, где
                // норма - это пара ВПП. Но пока что - еще не добавление,
                // а только флаг на его необходимость.
                need_to_insert = runway_number;
            }
        }

        if (( ! need_to_insert.empty() ) && ( nearest_founded_node.id >= 0 )) {
            // ВПП нужно добавить. И при этом мы действительно обнаружили какой-то
            // смысловой узел, наиболее близкий к уже существующей ВПП.
            land_runway_t rwy;
            rwy.runway_number = need_to_insert;
            location_t loc;
            loc.latitude = nearest_founded_node.latitude;
            loc.longitude = nearest_founded_node.longitude;
            taxi_network_routing_node_t far_away = get_farest_node(loc, nodes);
            rwy.end_latitude = far_away.latitude;
            rwy.end_longitude = far_away.longitude;
            _land_runways.push_back( rwy );
        }
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Положить один элемент traffic flow                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::_put_traffic_flow( traffic_flow_t ** ptr_traffic ) {
    traffic_flow_t * p_trafic = * ptr_traffic;
    if ( ! p_trafic ) return;
    _traffic_flow_.push_back( * p_trafic );
    delete( p_trafic );
    * ptr_traffic = nullptr;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Положить насчитанную стоянку                                               *
// *           (стоянка может содержать - несколько строк в файле, поэтому идет через динамический указатель).         *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::_put_startup_location( startup_location_t ** ptr_startup_location ) {
    auto pst = ( startup_location_t * ) * ptr_startup_location;
    if ( ! pst ) return;
    _startup_locations_[pst->name] = * pst;
    delete( pst );
    * ptr_startup_location = nullptr;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Положить ранее насчитанную taxi network                                         *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void Airport::_put_taxi_network( taxi_network_t ** ptr_taxi ) {
    auto p_taxi = * ptr_taxi;
    if ( ! p_taxi ) return;
    _taxi_networks_.push_back( * p_taxi );
    delete( p_taxi );
    * ptr_taxi = nullptr;
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Положить ранее насчитанный контейнер нодов, освободить память                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::_put_node_container( node_container_t ** ptr_container ) {
    auto container = * ptr_container;
    if ( ! container ) return;
    switch ( container->container_type ) {

        case CONTAINER_TAXIWAY: {
            auto ptwy = ( taxiway_t * ) container;
            _taxiways_.push_back( * ptwy );
        } break;

        case CONTAINER_LINEAR_FEATURE: {
            auto lft = ( linear_feature_t * ) container;
            _linear_features_.push_back( * lft );
        } break;

        case CONTAINER_BOUNDARY: {
            auto bdy = ( boundary_t * ) container;
            _boundaries_.push_back( * bdy );
        } break;

        default: throw runtime_error(
            "put node to container, unhandled type " + to_string(container->container_type)
        );
    }
    delete container;
    * ptr_container = nullptr;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Вернуть ближайший (-е) аэропорт (-ы)                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::get_nearest( location_t & location ) {
#ifdef INTERNAL_XPLANE
    XPLMNavRef dataref = XPLMFindFirstNavAidOfType( xplm_Nav_Airport );
    while ( dataref != XPLM_NAV_NOT_FOUND ) {
        // Получаем данные объекта, на который ссылается dataref.
        XPLMNavType navaid_type = xplm_Nav_Unknown;
        float latitude = 0.0, longitude = 0.0, height = 0.0, heading = 0.0;
        int frequency = 0;
        char id[32];
        char name[256];
        char region[1];
        XPLMGetNavAidInfo(
            dataref,
            &navaid_type,
            &latitude,
            &longitude,
            &height,
            &frequency,
            &heading,
            id,
            name,
            region
        );
        XPlaneUtilities::log( string("ID=") + string(id) + ", name=" + string(name) + ", type=" + to_string(navaid_type) );
        // Переход на следующий элемент.
        dataref = XPLMGetNextNavAid(dataref);
    }
#endif
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Чтение и парзинг всех имеющихся сценариев.                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void Airport::read_all() {
    // Без нее идут ошибки преобразования строк в числа.
    setlocale(LC_ALL, "C");

    string system_path = XPlane::get_system_path();
    string sep = XPlane::get_directory_separator();

    // Вектор полных путей к найденным файлам аэропортов.
    vector<string> founded;

    // Для отладки - один Кольцово.
    founded.push_back(
        system_path + sep + "Custom Scenery" + sep + "USSS_Koltsovo_2012" + sep + "Earth nav data" + sep + "apt.dat"
    );

//    // Аэропорты по умолчанию.
//    founded.push_back( system_path + sep + "Resources" + sep + "default scenery"
//        + sep + "default apt dat" + sep + "Earth nav data" + sep + "apt.dat"
//    );
//    founded.push_back( system_path + sep + "Custom Scenery" + sep + "Global Airports"
//        + sep + "Earth nav data" + sep + "apt.dat"
//    );
//
//    for (const auto & entry : fs::recursive_directory_iterator(system_path)) {
//        string file_name = entry.path().filename().string();
//        if ( file_name == "apt.dat" ) {
//            string entry_full_path = entry.path().string();
//            // Добавление пути происходит только в том случае, если его еще нет.
//            // Таким образом, аэропорты по умолчанию будут затерты теми, которые
//            // есть в Custom Scenery.
//            bool exists = false;
//            // Пробовал через find, почему-то не получил нужного мне результата. Переделал "в лоб".
//            for (const auto & elem: founded) {
//                if (elem == entry_full_path) {
//                    exists = true;
//                    break;
//                }
//            }
//
//            if ( ! exists ) {
//                founded.push_back( entry_full_path );
//            }
//        }
//    }

    // Пути - нашли. Теперь парзим каждый из этих аэропортов и
    // добавляем его в коллекцию. Возможно, замещая при этом.
    for (auto & path : founded) {
        try {
            Airport::read( path );
        } catch ( bad_format_exception & e ) {
            XPlane::log( "Airport parse file " + path + ": " + e.what() );
        } catch ( range_error & e ) {
            XPlane::log( "Airport parse file: " + path + ": " + e.what() );
        } catch ( runtime_error & e ) {
            XPlane::log( "Airport parse file " + path + ": " + e.what() );
        }
    }

    // Устанавливаем флаг того, что аэропорты были прочитаны.
    // Это - единственное место, где происходит установка данного флага.
    // Во всех остальных случаях он только читается.
    Airport::_airports_was_readed_ = true;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Выдать аэропорт по ICAO                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

xenon::Airport & Airport::get_by_icao( const string & icao_code ) {
    if (( ! Airport::_airports_.empty() ) && ( Airport::_airports_.contains(icao_code) ))
        return Airport::_airports_[icao_code];
    return Airport::_fake_airport_;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                     Вернуть ближайший к заданной локации узел из представленной коллекции узлов                   *
// *                                                                                                                   *
// *********************************************************************************************************************

Airport::taxi_network_routing_node_t Airport::get_nearest_node(
    const location_t & location, const vector<taxi_network_routing_node_t> & nodes
) {
    taxi_network_routing_node_t result;
    double min_delta = 10005000.0;
    for ( const auto & node : nodes ) {
        // По метрике L1, просто абсолютная разность. Так быстрее по времени.
        double delta = abs(location.latitude - node.latitude) + abs(location.longitude - node.longitude);
        if ( delta < min_delta ) {
            result = node;
            min_delta = delta;
        }
    }
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Получить наиболее удаленный узел                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

Airport::taxi_network_routing_node_t Airport::get_farest_node(
    const location_t & location, const vector<taxi_network_routing_node_t> & nodes
) {

    taxi_network_routing_node_t result;
    double max_delta = 0.0;
    for ( const auto & node : nodes ) {
        // По метрике L1, просто абсолютная разность. Так быстрее по времени.
        double delta = abs(location.latitude - node.latitude) + abs(location.longitude - node.longitude);
        if ( delta > max_delta ) {
            result = node;
            max_delta = delta;
        }
    }
    return result;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                 Вернуть ближайший узел taxi routing network                                       *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
Airport::taxi_network_routing_node_t Airport::get_nearest_taxi_network_node(
    const location_t & location, const string & like
) {
    taxi_network_routing_node_t result;
    double min_delta = 100500.0;
    for ( const taxi_network_routing_node_t & node: _taxi_network.nodes ) {
        double delta = abs(location.latitude - node.latitude) + abs(location.longitude - node.longitude);
        if ( delta < min_delta ) {
            int pos = 0;
            if ( ! like.empty() ) pos = node.name.find( like );
            if ( pos >= 0 ) {
                result = node;
                delta = min_delta;
            }
        }
    }
    return result;
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                              Получить узел по его внутриаэропортовому идентификатору                              *
// *                                                                                                                   *
// *********************************************************************************************************************

Airport::taxi_network_routing_node_t Airport::get_taxi_network_node( const int & id ) {
    for ( const auto & node : _taxi_network.nodes ) {
        if ( node.id == id ) return node;
    }
    return taxi_network_routing_node_t();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Получить узел по его имени                                               *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
Airport::taxi_network_routing_node_t Airport::get_taxi_network_node_by_name( const string & name ) {
    for ( const auto & node: _taxi_network.nodes ) {
        if ( node.name == name ) return node;
    }
    return taxi_network_routing_node_t();
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                              Получить узлы, содержащие определенное вхождение в имя                               *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
vector<Airport::taxi_network_routing_node_t> Airport::get_taxi_network_nodes_like( const string & name ) {
    vector<Airport::taxi_network_routing_node_t> result;
    for ( const auto & node: _taxi_network.nodes ) {
        int pos = node.name.find( name );
        if ( pos >= 0 ) result.push_back( node );
    }
    return result;
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Получить узлы, имеющие данную активную зону                                    *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
vector<Airport::taxi_network_routing_edge_t> Airport::get_taxi_network_edges_with_active_zone(
    const string &active_zone
) {
    vector <taxi_network_routing_edge_t> result;
    for ( const auto & edge : __taxi_network.edges ) {
        for ( const auto & zone : edge.active_zones ) {
            if (
                    ( edge.type == "runway" )
                    && ( zone.classification == "departure" )
                    && ( zone.runways.find(active_zone) >= 0 )
            )   result.push_back( edge );
        }
    }
    return result;
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Вернуть взлетку                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
Airport::land_runway_t Airport::get_land_runway( const string & name ) {
    for ( const auto & rwy : _land_runways ) {
        if ( rwy.runway_number == name ) return rwy;
    }
    return land_runway_t();
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Получить соседей этого узла                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

vector< Airport::taxi_network_routing_node_t> Airport::get_neighbors( const taxi_network_routing_node_t & node ) {
    vector< Airport::taxi_network_routing_node_t > result;
    for ( const auto & edge : _taxi_network.edges ) {
        if ( edge.start_id == node.id ) {
            // Добавляем узел, который является концом данного edge.
            result.push_back( get_taxi_network_node(edge.end_id) );
        } else if ( edge.end_id == node.id ) {
            result.push_back( get_taxi_network_node( edge.start_id ));
        }
    }
    return result;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Вернуть отсортированную коллекцию узлов для исходной коллекции edges                        *
// *                                                                                                                   *
// *********************************************************************************************************************

vector< Airport::taxi_network_routing_node_t > Airport::get_nodes_for(
    const vector<taxi_network_routing_edge_t> &edges
) {
    vector< taxi_network_routing_node_t> nodes;

    // Складываем только в том случае, если узла еще нет.
    // Небольшая встроенная процедурка на эту тему.
    auto has = [](int node_index, const vector <taxi_network_routing_node_t> & nodes ) {
        for ( const auto & n : nodes )
            // true - такой узел у нас уже есть.
            if ( n.id == node_index ) return true;
        // false - это значит, что такого узла у нас еще нет.
        return false;
    };

    // Цикл по полученным edges.
    for ( const auto & edge : edges ) {

        if ( ! has (edge.start_id, nodes )) {
            auto node_start = get_taxi_network_node( edge.start_id );
            nodes.push_back( node_start );
        }
        if ( ! has ( edge.end_id, nodes )) {
            auto node_end = get_taxi_network_node( edge.end_id );
            nodes.push_back( node_end );
        }
    }

    // Сортировка массива nodes, ключом является сумма latitude + logitude.
    sort( nodes.begin(), nodes.end(), []( taxi_network_routing_node_t & a, taxi_network_routing_node_t & b ) {
        return ( a.latitude + a.longitude ) > ( b.latitude + b.longitude );
    });

    return nodes;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                 Сгруппировать все узлы по их имени, если их тип содержит определенное значение                    *
// *                        Используется для определения runway и taxiway из taxi network route                        *
// *                                                                                                                   *
// *********************************************************************************************************************

map<string, vector<Airport::taxi_network_routing_edge_t>> Airport::_group_taxi_network_edges( const string & type ) {
    map<string, vector<Airport::taxi_network_routing_edge_t>> result;
    for ( const auto & edge: _taxi_network.edges ) {
        if ( edge.type == type ) {
            if (result.contains(edge.name)) result[edge.name].push_back(edge);
            else {
                vector<Airport::taxi_network_routing_edge_t> edges;
                edges.push_back(edge);
                result[edge.name] = edges;
            }
        }
    }
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Вернуть (в routing edges) все взлетки                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

map<string, vector<Airport::taxi_network_routing_edge_t>> Airport::get_runways_edges() {
    return _group_taxi_network_edges("runway");
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Вернуть (в route edges) все рулежки                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

map< string, vector<Airport::taxi_network_routing_edge_t>> Airport::get_taxiways_edges() {
    return _group_taxi_network_edges( "taxiway" );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Вернуть используемые ВПП с указанием их функции (departure, arrival)                       *
// *                                                                                                                   *
// *********************************************************************************************************************

vector<runway_in_use_t> Airport::get_runways_in_use() {
    vector< runway_in_use_t > result;
    // На пока что заглушка.
    runway_in_use_t r1{
        .name =  "08L",
        .used =  RUNWAY_USED_ARRIVAL
    };
    result.push_back(r1);
    r1.used = RUNWAY_USED_DEPARTURE;
    result.push_back(r1);
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                 Получить ближайший к указанному положению узел, который принадлежит именно к рулежкам             *
// *                                                                                                                   *
// *********************************************************************************************************************

Airport::taxi_network_routing_node_t Airport::get_nearest_taxiway_node(const location_t & location) {

    // Нам нужен ближайший traxiway к тому месту, где сейчас находится самолет.
    auto twys = get_taxiways_edges();
    double min_distance = 100500.0;
    taxi_network_routing_node_t nearest_node;
    vector< taxi_network_routing_node_t > nearest_taxiway;
    string taxiway_name;

    for ( const auto & twy : twys ) {
        string name = twy.first;
        auto edges = twy.second;
        auto nodes = get_nodes_for(edges);
        auto n_node = get_nearest_node(location, nodes);
        double delta = abs(n_node.latitude - location.latitude) + abs(n_node.longitude - location.longitude);
        if ( delta < min_distance ) {
            min_distance = delta;
            nearest_node = n_node;
            nearest_taxiway = nodes;
            taxiway_name = name;
        }
    }

    return nearest_node;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                   Получить объект ВПП для проведения конкретного действия (взлета или посадки)                    *
// *                                                                                                                   *
// *********************************************************************************************************************

Airport::land_runway_t Airport::get_runway_for( const runway_used_t & use ) {

    auto rwys_in_use = get_runways_in_use();
    land_runway_t result;
    for (const auto & rwy : rwys_in_use ) {
        if (( rwy.used == use ) || ( rwy.used == RUNWAY_USED_BOTH )) {
            for ( const auto & er: _land_runways ) {
                if (er.runway_number == rwy.name) {
                    result = er;
                    break;
                }
            }
            break;
        }
    }
    return result;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Получить все ребра, в которые входит данный узел                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

vector< Airport::taxi_network_routing_edge_t > Airport::get_edges_for(const taxi_network_routing_node_t &node) {
    vector< taxi_network_routing_edge_t > result;
    for ( const auto & edge : _taxi_network.edges ) {
        if ( ( edge.start_id == node.id ) || ( edge.end_id == node.id ) )
            result.push_back( edge );
    }
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *        Выдать целевую точку и целевой курс для выталкивания или выруливания самолета из его текущей позиции.      *
// *                                                                                                                   *
// *********************************************************************************************************************

vector<location_t> Airport::get_taxi_way_for_departure( const location_t & from ) {

    vector<location_t> result;

    // ВПП в использовании для взлета.
    land_runway_t departure_rwy = get_runway_for( RUNWAY_USED_DEPARTURE );
    if (departure_rwy.runway_number.empty()) {
        XPlane::log("Airport::get_start_location_for_departure_taxing, empty runway name for departure.");
        return result;
    }

    // Ближайший к заданному положению узел, принадлежащий именно рулежным дорожкам.
    auto nearest_node_descriptor = __routes.get_nearest_node( from, AirportNetwork::WAY_TAXIWAY );
    AirportNetwork::node_t nearest_node;
    try {
        nearest_node = __routes.graph()[ nearest_node_descriptor ];
    } catch ( const range_error & re ) {
        XPlane::log(
            "Airport::get_start_for_departure_taxing, no nearest node found for lat="
            + to_string(from.latitude) + ", lon=" + to_string(from.longitude)
        );
        return result;
    }

    // Кратчайший путь от найденного узла до взлетки.
    auto path = __routes.shortest_path( nearest_node_descriptor, departure_rwy.location() );
    for ( auto nd: path ) {
        auto node_itself = __routes.graph()[nd];
        result.push_back( node_itself.location );
    }

    return result;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                 Итеративный поиск следующей ближайшей точки пути для достижения конкретной локации                *
// *                                                                                                                   *
// *********************************************************************************************************************

location_with_angles_t Airport::get_next_nearest_path_item(
    const location_t & from, const location_t & to
) {
    location_with_angles_t result;

    // Переданная в качестве параметра точка может быть вообще случайной. Поэтому
    // для начала нужно найти "стартовый узел", от которого будем прокладывать курс.
    auto start_node = get_nearest_taxiway_node( from );

    if ( start_node.id < 0 ) return result;

    auto edges = get_edges_for( start_node );
    for ( const auto & edge : edges ) {
        if ( edge.type != "taxiway") return result;
    }
    // Соседи текущего узла.
    auto neighbors = get_neighbors(start_node);
    // Ближайший из них к ВПП.
    auto nearest = get_nearest_node(to, neighbors);
    float heading = ( float ) XPlane::bearing(from, nearest.location());

    result.location = nearest.location();
    result.rotation.heading = heading;
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Просчитать координаты точек и курсы в этих точках - для руления с целью вылета                     *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
vector<location_with_angles_t> Airport::get_path_for_departure_taxing( const location_t & location ) {
    vector< location_with_angles_t > way;
    // ВПП для вылета.
    auto runway = get_runway_for(RUNWAY_USED_DEPARTURE);
    location_t runway_location = {
        .latitude = runway.end_latitude,
        .longitude = runway.end_longitude,
        .altitude = 10.0
    };

    auto one_path_step = get_next_nearest_path_item( location, runway_location );

    while ( ( one_path_step.location.latitude != 0.0 )
        && ( one_path_step.location.longitude != 0.0 )
    ) {
        way.push_back( one_path_step );
        one_path_step = get_next_nearest_path_item( one_path_step.location, runway_location );
    }

    return way;
}
*/
