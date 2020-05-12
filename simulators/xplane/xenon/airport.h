// *********************************************************************************************************************
// *                                                     Аэропорт.                                                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 12:41 *
// *********************************************************************************************************************
#pragma once

#include <map>
#include <string>
#include <vector>

#include <lemon/list_graph.h>

#include "structures.h"
#include "xplane_utilities.h"
#include "airport_network.h"

using namespace std;
using namespace lemon;

namespace xenon {

#pragma mark Типы аэропортов.
    const int AP_TYPE_UNKNOWN = 0;
    const int AP_TYPE_LAND = 1;
    const int AP_TYPE_SEA = 16;
    const int AP_TYPE_HELI = 17;

#pragma mark Типы частот аэропорта.
    const int FREQ_NONE = 0;
    const int FREQ_ATC_RECORDED = 1050;     const int FREQ_OLD_ATC_RECORDED = 50;
    const int FREQ_ATC_UNICOM = 1051;       const int FREQ_OLD_ATC_UNICOM = 51;
    const int FREQ_ATC_CLD = 1052;          const int FREQ_OLD_ATC_CLD = 52;
    const int FREQ_ATC_GND = 1053;          const int FREQ_OLD_ATC_GND = 53;
    const int FREQ_ATC_TWR = 1054;          const int FREQ_OLD_ATC_TWR = 54;
    const int FREQ_ATC_APP = 1055;          const int FREQ_OLD_ATC_APP = 55;
    const int FREQ_ATC_DEP = 1056;          const int FREQ_OLD_ATC_DEP = 56;

#pragma mark Типы элементов, которые могут содержать в себе узлы.
    const int CONTAINER_NONE = 0;
    const int CONTAINER_TAXIWAY = 110;
    const int CONTAINER_LINEAR_FEATURE = 120;
    const int CONTAINER_BOUNDARY = 130;

#pragma mark Типы нодов.
    const int NODE_UNKNOWN = 0;
    const int NODE_PLAIN = 111;
    const int NODE_BEZIER = 112;
    const int NODE_CLOSE_BOUNDARY = 113;
    const int NODE_CLOSE_BEZIER = 114;
    const int NODE_END_LINE = 115;
    const int NODE_END_BEZIER = 116;

    class Airport {

        public:
            /**
             * @short Наземная ВПП (RWY)
             */
            struct land_runway_t {
                // Width of runway in metres
                float width = 0.0;
                // Code defining the surface type (concrete, asphalt, etc) Integer
                // value for a Surface Type Code (see below)
                int surface_type = 0;
                // Code defining a runway shoulder surface type
                // 0=no shoulder, 1=asphalt shoulder, 2=concrete shoulder
                int shoulder_surface_type = 0;
                // Runway smoothness (not used by X-Plane yet)
                // 0.00 (smooth) to 1.00 (very rough). Default is 0.25
                float smoothness = 0.0;
                // Runway centre-line lights
                // 0=no centerline lights, 1=centre line lights
                int centre_line_light = 0;
                // Runway edge lighting (also implies threshold lights)
                // 0=no edge lights, 2=medium intensity edge lights
                int edge_light = 0;
                // Auto-generate distance-remaining signs (turn off if created manually)
                // 0=no auto signs, 1=auto-generate signs
                int distance_remaining_sign = 0;
                // Runway number (eg. “31R”, “02”). Leading zeros are required.
                // Two to three characters. Valid suffixes: “L”, “R” or “C” (or blank)
                string runway_number = "";
                // Latitude of runway end (on runway centerline) in decimal degrees
                // Eight decimal places supported
                double end_latitude = 0.0;
                // Longitude of runway end (on runway centerline) in decimal degrees
                // Eight decimal places supported
                double end_longitude = 0.0;
                // Length of displaced threshold in metres (this is included in implied runway length)
                // A displaced threshold will always be inside (between) the two runway ends
                //Two decimal places (metres). Default is 0.00
                float displaced_threshold_length = 0.0;
                // Length of overrun/blast-pad in metres (not included in implied runway length)
                // Two decimal places (metres). Default is 0.00
                float overrun_length = 0.0;
                // Code for runway markings (Visual, non-precision, precision)
                // Integer value for Runway Marking Code (see below)
                int markings = 0;
                // Code for approach lighting for this runway end Integer
                // value for Approach Lighting Code (see below)
                int approach_ligth = 0;
                // Flag for runway touchdown zone (TDZ) lighting
                // 0=no TDZ lighting, 1=TDZ lighting
                int tdz_ligth = 0;
                // Code for Runway End Identifier Lights (REIL)
                // 0=no REIL, 1=omni-directional REIL, 2=unidirectional REIL
                int reil_light = 0;

                location_t location() {
                    return location_t {
                        .latitude = end_latitude,
                        .longitude = end_longitude
                    };
                }
            };

            struct water_runway_t {
                // Width of runway in metres Two decimal places recommended. Must be >= 1.00
                float width = 0.0;
                // Flag for perimeter buoys 0=no buoys, 1=render buoys
                int perimeter_buoys = 0;
                // Runway number. Not rendered in X-Plane (it’s on water!)
                // Valid suffixes are “L”, “R” or “C” (or blank)
                string runway_number = "";
                // Latitude of runway end (on runway centerline) in decimal degrees
                // Eight decimal places supported
                double latitude = 0.0;
                // Longitude of runway end (on runway centerline) in decimal degrees
                //Eight decimal places supported
                double longitude = 0.0;
            };

            struct helipad_runway_t {
                // Designator for a helipad. Must be unique at an airport.
                // Usually “H” suffixed by an integer (eg. “H1”, “H3”)
                string designator = "";
                // Latitude of helipad centre in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of helipad centre in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Orientation (true heading) of helipad in degrees Two decimal places recommended
                float orientation = 0.0;
                // Helipad length in metres Two decimal places recommended (metres), must be >=1.00
                float length = 0.0;
                // Helipad width in metres Two decimal places recommended (metres), must be >= 1.00
                float width = 0.0;
                // Helipad surface code Integer value for a Surface Type Code (see below)
                int surface_code = 0;
                // Helipad markings 0 (other values not yet supported)
                int markings = 0;
                // Code defining a helipad shoulder surface type
                // 0=no shoulder, 1=asphalt shoulder, 2=concrete shoulder
                int surface_type = 0;
                // Helipad smoothness (not used by X-Plane yet) 0.00 (smooth) to 1.00 (very rough). Default is 0.25
                float smoothness = 0.0;
                // Helipad edge lighting 0=no edge lights, 1=yellow edge lights
                int edge_light = 0;
            };

            struct viewpoint_t { // code = 14
                // Latitude of viewpoint in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of viewpoint in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Height (in feet) of viewpoint above ground level Integer
                int height = 0;
                // Name of viewpoint (not used by X-Plane) Descriptive text string (optional)
                string name = "";
            };

            struct light_beacon_t { // 18
                // Latitude of beacon in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of beacon in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Code for type of light beacon. Determines colors of beacon.
                // Integer Beacon Type Code (see below)
                int type_code = 0.0;
                // Name of viewpoint (not used by X-Plane) Descriptive text string (optional)
                string name = "";

            };

            struct windsock_t { // code = 19
                // Latitude of windsock in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of windsock in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Flag for windsock lighting 0=unlit, 1=illuminated
                int flag_lighting = 0;
                // Name of viewpoint (not used by X-Plane) Descriptive text string (optional)
                string name = "";
            };

            struct sign_t { // code = 20
                // Latitude of sign in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of sign in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Orientation of sign in true degrees (heading of someone looking at sign’s front)
                // Two decimal places recommended
                float orientation = 0.0;
                // Code for sign size Integer Sign Size Code (see below)
                int size_code = 0;
                // Text to be rendered on sign front and/or back Text string
                // formatted by Sign Text Definition (see below)
                string label = "";
            };

            struct lighting_objects_t { // code = 21
                // Latitude of lighting object in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of lighting object in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Code for type of lighting object Integer Lighting Object Code (see below)
                int object_type = 0;
                // Orientation of lighting object in true degrees (looking toward object)
                // Two decimal places recommended
                float orientation = 0.0;
                // Visual glideslope angle in degrees Two decimal places.
                // 0.00 if not required. Default is 3.00
                float glidescope_angle = 0.0;
                // Associated runway number (required for VASI/PAPI, etc)
                // One to three characters
                string runway_number = "";
                // Description of lighting object (not used by X-Plane Short text string (optional)
                string description = "";
            };

            struct traffic_flow_wind_rule_t { // 1001
                // METAR reporting station (may be a remote airport, eg KSEA for KBFI)
                // ICAO code, up to 7 characters
                string metar_station = "";
                // Wind direction minimum (magnetic) 000 - 359
                int wind_direction_minimum = 0;
                // Wind direction maximum (magnetic) 000 - 359
                int wind_direction_maximum = 0;
                // Maximum wind speed. Use 999 for ‘all’ wind speeds. 0 - 999
                int wind_speed_maximum = 0;
            };

            struct traffic_flow_ceiling_rule_t { // 1002
                // METAR reporting station (may be a remote airport, eg KSEA for KBFI)
                // ICAO code, up to 7 characters
                string metar_station = "";
                // Minimum reported ceiling in feet AGL at reporting station
                // Positive integer
                int minimum_height = 0;
            };

            struct traffic_flow_visibility_rule_t { // 1003
                // METAR reporting station (may be a remote airport, eg KSEA for KBFI)
                // ICAO code, up to 7 characters
                string metar_station = "";
                // Minimum reported visibility in statute miles Float (eg. “1.5”)
                float minimum_visibility = 0.0;
            };

            struct traffic_flow_runway_in_use_t { // 1100, 1110
                // Runway end identifier Two to three characters. Valid suffixes: “L”, “R” or “C” (or blank)
                string runway = "";
                // Arrival or departure frequency. rounded DOWN where necessary, 1kHz resolution
                int frequency;
                // Rule type (arrivals, departures) Pipe separated list (“|”). ‘arrivals’ and/or ‘departures’
                string rule_type = "";
                // Airplane types to which rule applies Pipe-separated list (“|”).
                // Can include “heavy”, ”jets”, “turboprops”, “props” and “helos”
                string airplane_types = "";
                // On course heading range ((ie. first leg of flight plan
                // for departures, last leg for arrivals)
                // 000000 - 359359
                string on_course_heading = "";
                // Initial ATC assigned departure heading range. Not used for arrivals.
                // 000000 - 359359
                string atc_heading = "";
                // Rule name Descriptive name (max 50 characters)
                string name = "";
            };

            struct traffic_flow_t { // 1000
                string name = "";
                vector< traffic_flow_wind_rule_t > wind_rules; // 1001
                vector< traffic_flow_ceiling_rule_t > ceiling_rules; // 1002
                vector< traffic_flow_visibility_rule_t> visibility_rules; // 1003
                string time_from = "";
                string time_to = "";
                // RWY - direction
                map<string, string> vfr_pattern;
                vector< traffic_flow_runway_in_use_t > runway_in_use;
            };

            struct frequency_t { // 1050 - 1056, 50-56
                // Сам исходный код того, что привело к образованию "частоты".
                int freq_type = FREQ_NONE;
                int frequency = 0.0;
                string description = "";
            };

            struct taxi_network_routing_node_t {
                // Latitude of node in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of node in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Usage of node in network (begin or end a taxi path, or both)
                // “dest”, “init”, “both” or “junc”
                string usage;
                // Node identifier (defined in 0 based sequence, ascending)
                // Integer. Must be unique within scope of an airport.
                // Минус 1 означает, что реально узла - нет.
                int id = -1;
                // Node name. Not currently used. String (max 16 characters)
                string name = "";

                location_t location() {
                    return location_t {
                        .latitude = latitude,
                        .longitude = longitude
                    };
                };
            };

            struct taxi_network_routing_edge_active_zone_t {
                string classification;
                string runways;
            };

            struct taxi_network_routing_edge_t {
                // Node identifier for start of edge Integer. Must refer to valid node (row code ‘1201’)
                int start_id = 0;
                // Node identifier for end of edge Integer. Must refer to valid node (row code ‘1201’)
                int end_id = 0;
                // Edge can be used in both directions “twoway” or “oneway”
                string directions = "";
                // Node is on a regular taxiway. If on “runway” a clearance is needed from ATC
                // “taxiway” or “runway”
                string type = "";
                // Taxiway identifier. Used to build ATC taxi clearances (eg. “.. .taxi via A, T, Q”)
                // String. Taxiway or runway identifier (eg. “A” or “16L/34R”)
                string name = "";
                vector< taxi_network_routing_edge_active_zone_t > active_zones;
            };

            struct taxi_network_t {
                vector< taxi_network_routing_node_t > nodes;
                vector< taxi_network_routing_edge_t > edges;
                vector< taxi_network_routing_edge_t > ground_vehicle_edges;
            };

            struct truck_parking_t { // 1400
                // Latitude of location in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of location in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Heading (true) of the OBJ positioned at this location Decimal degrees
                float heading = 0.0;
                // type string baggage_loader, baggage_train, crew_car, crew_ferrari, crew_limo, pushback,
                // fuel_liners, fuel_jets, fuel_props, food, gpu
                string parking_type = "";
                // 0 to 10 if type is baggage_train, 0 if not
                int baggage_train_type = 0.0;
                // Name of parking Text string
                string name = "";
            };

            struct truck_destination_t { // 1401
                // Latitude of location in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // Longitude of location in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // Heading (true) of the positioned at this location Decimal Degrees, true heading
                float heading = 0.0;
                // Truck types allowed to end up at this destination Pipe separated list (“|”).
                // Include 1 or more of the following baggage_loader,
                // baggage_train, crew_car, crew_ferrari, crew_limo, pushback, fuel_liners, fuel_jets,
                // fuel_props, food, gpu
                string truck_types = "";
                // Name of Truck Destination Text string
                string name = "";
            };

            struct node_t {
                // Row code for a node. First node must follow an appropriate header row
                // 111 thru 116
                int node_type = NODE_UNKNOWN;
                // [All nodes] Latitude of node in decimal degrees Eight decimal places supported
                double latitude = 0.0;
                // [All nodes] Longitude of node in decimal degrees Eight decimal places supported
                double longitude = 0.0;
                // [112, 114, 116 only] Latitude of Bezier control point in decimal degrees
                // Eight decimal places supported. Ignore for 111, 113, 115
                double bezier_point_latitude = 0.0;
                // [112, 114, 116 only] Latitude of Bezier control point in decimal degrees
                // Eight decimal places supported. Ignore for 111, 113, 115
                double bezier_point_longitude = 0.0;
                // [Not for 115 or 116] Code for painted line type on line segment starting at this node
                // Integer Line Type Code (see below). Not for 115 or 116
                int line_type_code = 0;
                // [Not for 115 or 116] Code for lighting on line segment starting at this node
                // Integer Line Type Code (see below). Not for 115 or 116
                int lighting_type_code = 0;

            };

            struct node_container_t {
                int container_type = CONTAINER_NONE;
                vector< node_t > nodes;
            };

            /**
             * @short Taxiways, node container.
             */
            struct taxiway_t : public node_container_t {
                // Code defining the surface type (concrete, asphalt, etc)
                // Integer value for a Surface Type Code (see below)
                int surface_type = 0;
                // Runway smoothness (not used by X-Plane yet)
                // 0.00 (smooth) to 1.00 (very rough). Default is 0.25
                float smoothness = 0.0;
                // Orientation (true degrees) of pavement texture ‘grain’ Two decimal places recommended
                float orientation = 0.0;
                // Description of pavement chunk (not used by X-Plane) Text string
                string description = "";
            };

            /**
             * @short Linear feature, node container.
             */
            struct linear_feature_t : public node_container_t {
                string name;
            };

            /**
             * @short Airport boundary, node container.
             */
            struct boundary_t : public node_container_t {
                // Description of feature or boundary (not used by X-Plane). Text string.
                string description;
            };

            Airport();
            Airport( const Airport & apt ) {
                * this = apt;
            };
            Airport & operator = ( const Airport & apt ) = default;
            virtual ~Airport() = default;
            // Airport & operator = ( const Airport & apt );
            // Получить ближайший по координатам.
            static void get_nearest( location_t & location );
            // Получить по ICAO
            static Airport & get_by_icao( const string & icao_code );
            // Чтение всех имеющихся сценариев.
            static void read_all();

            /**
             * @short Прочитать один файл формата apt.dat и распарзить его.
             * https://developer.x-plane.com/article/airport-data-apt-dat-file-format-specification/
             * Возвращаемого значения нет, вместо этого функция замещает элемент в Airport::_airports_. Аэропорт
             * в файле может быть не один, парзится - либо до окончания файла, либо до получения типа 99
             * (признак конца файла).
             *
             * @throw xenon::bad_format_exception В этом случае дальнейшая обработка файла прекращается,
             * но при этом в словарь Airport::_airports_ будут записаны те аэропорты, которые успели распарзится.
             * @throw runtime_error если были ошибки в логике парзинга.
             */
            static void read( const string & full_path_to_apt_dat );

            /** @short Вернуть общее количество распарзенных аэропортов.
             *
             * @return
             */
            static int count() {
                return Airport::_airports_.size();
            };

            /**
             * @short Вернуть флаг прочитанности всех аэропортов (т.е. инициализированности класса).
             * @return
             */
            static bool airports_was_readed() {
                return _airports_was_readed_;
            };

            string icao_code() {
                return _icao_code_;
            };

            /**
             * Установка указателя на cb-функцию после распознавания аэропорта.
             * @param reaction
             */
            static void set_has_been_parsed( void ( * reaction )( string & icao_code ) ){
                Airport::_has_been_parsed_ = reaction;
            }

            /**
             * @short "Осиротевшесть" аэропорта.
             * @return
             * @see _orphanded
             */

            bool is_orphanded() {
                return _orphanded;
            }

            /**
             * @short Вернуть ссылку на все имеющиеся стоянки.
             * @return
             */

            map< string, startup_location_t > & get_startup_locations() {
                return _startup_locations_;
            }

            /**
             * @short Вернуть используемые ВПП c указанием их выполняемой функции (departure, arrival)
             * @return
             */
            vector<runway_in_use_t> get_runways_in_use();

            // Получить ближайший к координатам узел.
            // taxi_network_routing_node_t get_nearest_taxi_network_node(
            //    const location_t & location, const string & like = ""
            // );
            /**
             * @short Получить ближайщий (по метрике L1) к данной локации узел из представленной коллекции.
             * @param location Местоположение в гео-координатах, для которого выбираем ближайший узел.
             * @param nodes Предварительно подготовленная по какому-либо признаку коллекция узлов. Например,
             * это может быть конкретный taxiway или runway.
             * @return
             */
            taxi_network_routing_node_t get_nearest_node(
                const location_t & location, const vector<taxi_network_routing_node_t> & nodes
            );

            /**
             * @short Получить самый дальний (по метрике L1) узел.
             * @param location Местоположение в гео-координатах, по отношению к которому ищем наиболее удаленный узел.
             * @return
             */
            taxi_network_routing_node_t get_farest_node(
                const location_t & location, const vector<taxi_network_routing_node_t> & nodes
            );

            // Получить узел по его внутриаэропортовому идентификатору.
            taxi_network_routing_node_t get_taxi_network_node( const int & id );

            // taxi_network_routing_node_t get_taxi_network_node_by_name( const string & name );
            // vector <taxi_network_routing_node_t> get_taxi_network_nodes_like( const string & name );

            // Получить соседей узла.
            vector< taxi_network_routing_node_t> get_neighbors(const taxi_network_routing_node_t & node );

            // land_runway_t get_land_runway(const string & name );

            map< string, vector< taxi_network_routing_edge_t >> get_taxiways_edges();
            map< string, vector< taxi_network_routing_edge_t >> get_runways_edges();

            /**
             * @short Получить все ребра, в которые входит этот узел.
             * @param node
             * @return
             */
            vector<taxi_network_routing_edge_t> get_edges_for( const taxi_network_routing_node_t & node );

            /**
             * @short Вернуть все узлы, входящие в данную коллекцию edges.
             * Гарантированно каждый узел в возвращаемом результате будет присутствовать только один раз.
             * Дополнительно узлы в возвращаемом векторе сортируются по сумме latitude + lognitude.
             * @return
             */
            vector<taxi_network_routing_node_t> get_nodes_for(const vector<taxi_network_routing_edge_t> & edges);

            /**
             * @short Получить ближайший к указанному положению узел, который принадлежит именно к рулежкам.
             * @param location
             * @return
             */
            taxi_network_routing_node_t get_nearest_taxiway_node( const location_t & location );

            /**
             * Получить объект ВПП для конкретного действия (взлета или посадки).
             * @param use
             * @return
             */
            land_runway_t get_runway_for( const runway_used_t & use );

            /**
             * @short Итеративный поиск следующей ближайшей точки пути.
             * @param target
             * @param node
             * @return
             */

            location_with_angles_t get_next_nearest_path_item(
                const location_t & from, const location_t & to
            );

            /**
             * @short Выдать целевое конечное положение при выталкивании или выруливании.
             * @param from точка, где сейчас самолет находится. Курс для расчета конечной (целевой) точки -
             * не имеет значения, т.к. первоначальный сегмент все равно будет прямолинейным. Курс влияет
             * на то, будет ли самолет выруливать сам или его надо выталкивать, но это решает уже сам
             * самолет.
             * @return location_with_angles_t, содержащая
             *   - .location Координаты целевой точки, где самолет (плюс-минус) должен остановиться.
             *   - .rotation.heading @param target_heading целевой курс. На целевой точке самолет должен оказаться - с этим курсом
             * (опять же плюс-минус).
             */
            location_with_angles_t get_start_location_for_departure_taxing( const location_t & from );
            vector<location_with_angles_t> get_path_for_departure_taxing( const location_t & location );

        private:

            // Общая кучка прочитанных аэропортов. Ключом является код аэропорта,
            // таким образом, более поздние замещают более ранних.
            static map<string, Airport> _airports_;
            static bool _airports_was_readed_;
            // Пустышка. Ссылка на нее выдается в том случае,
            // если не был найден запрошенный аэропорт.
            static xenon::Airport _fake_airport_;

            // На всякий случай запоминается полный путь к исходному распарзенному файлу.
            string _full_path_to_apt_dat_;
            string _origin_;
            int _version_;
            int _ap_type_;
            int _evalution_in_feet_;
            string _icao_code_;
            string _name_;
            viewpoint_t _viewpoint_;

            // Атрибуты аэропорта. Могут быть, а могут не быть.
            string _city_;
            string _country_;
            double _datum_lon_;
            double _datum_lat_;
            string _gui_label_;
            string _iata_code_;
            string _region_code_;
            string _state_;
            string _transition_alt_;
            string _transition_level_;
            string _faa_code_;
            int _flatten_;
            bool _drive_on_left_;
            string _local_code_;

            /**
             * @short "Осиротевшесть" аэропорта внутри X-Plane.
             * В нормальном состоянии аэропорт представляет собой отдельно действующего агента,
             * который общается с X-Plane через сервер или P2P. Если внешний агент недоступен,
             * то аэропорт внутри X-Plane - "осиротел" и действует сам по себе.
             */
            bool _orphanded;

            // Реакция на парзинг одного аэропорта.
            static void ( * _has_been_parsed_ )( string & icao_code );

            // Граф taxi network внутри объекта аэропорта.
            xenon::AirportNetwork _routes;

            // Объекты внутри аэропорта.
            vector< land_runway_t > _land_runways;
            vector< water_runway_t > _water_runways;
            vector< helipad_runway_t > _helipad_runways;
            // Ключ - имя стоянки. Элемент - сама стоянка.
            map< string, startup_location_t > _startup_locations_;
            vector< truck_parking_t > _truck_parkings_;
            vector< truck_destination_t > _truck_destinations_;

            // node container's
            vector< light_beacon_t > _light_beacons_;
            vector< frequency_t > _frequencies_;
            vector< windsock_t > _windsocks_;
            vector< sign_t > _signs_;
            vector< lighting_objects_t > _lighting_objects_;
            vector< taxiway_t > _taxiways_;
            vector< linear_feature_t > _linear_features_;
            vector< boundary_t > _boundaries_;

            // Traffic flow rules
            vector <traffic_flow_t> _traffic_flow_;

            // Taxi routing network
            // TODO: кандидат на удаление
            taxi_network_t _taxi_network;

            // Положить насчитанный контейнер в аэропорт и освободить динамически созданный блок в памяти.
            void _put_node_container( node_container_t ** ptr_container );
            // void _put_taxi_network( taxi_network_t ** ptr_taxi );
            void _put_traffic_flow( traffic_flow_t ** ptr_traffic );
            void _put_startup_location( startup_location_t ** ptr_startup_location );
            static void _put_airport( Airport ** ptr_airport );

            /**
             * @short Получить edges, сгруппированных по имени, при условии, что их тип имеет определенное значение.
             *
             * Получить все edges, например, для рулежных дорожек. Они будут сгруппированы по имени рулежной дорожки.
             * Это дает возможность выбирать рулежные дорожки непосредственно из taxi route network.
             *
             * @param type Тип, который ищем (например runway или taxiway).
             * @return map<имя, вектор<edges>>
             */

            map < string, vector<taxi_network_routing_edge_t> > _group_taxi_network_edges( const string & type );

            /**
             * @short Проверить "полноту" заполнения ВПП.
             * Бывает, что взлетки в файле - все полностью. А бывает, что дают только один конец этих взлеток,
             * второй нужно вычислять по taxi network. Функция дополняет ВПП аэропорта, если их не хватает.
             */

            void _check_runway_fullness();

    };

};
