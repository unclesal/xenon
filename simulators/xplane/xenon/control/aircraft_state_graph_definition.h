// *********************************************************************************************************************
// *                                          Определения для графа состояний самолета                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 10:56 *
// *********************************************************************************************************************

#pragma once

#include <string>
#include <boost/graph/adjacency_list.hpp>

#include "structures.h"

namespace xenon {
    
    namespace aircraft_state_graph {
                
        // Узел графа - состояние самолета.
        struct node_t {
            // Описание состояния
            aircraft_state_t state = ACF_STATE_UNKNOWN;
            // Указатель на класс состояния, AbstractState.
            // Но его тип здесь специфицировать нельзя, иначе
            // получим циклическую ссылку инклудников.
            void * ptr_state_class = nullptr;
            bool current_state = false;
            std::string name;
        };

        // Ребро графа - действие для перехода в другое состояние.
        struct edge_t {
            aircraft_action_t action = ACF_DOES_NOTHING;
            // Указатель на класс действия.
            void * ptr_does_class = nullptr;
            bool current_action = false;
            std::string name;
        };

        // Описание собственно графа.
        typedef boost::adjacency_list<
            // selects the STL list container to store the OutEdge list
            boost::listS,
            // selects the STL vector container to store the vertices
            boost::vecS,
            boost::directedS,
            node_t, edge_t
        > graph_t;

        // Параметры действия, чтобы их можно было передавать следующему 
        // выбранному действию от предыдущего (для обеспечения плавности).
        
        struct action_parameters_t {            
            // "Рывок", производная от ускорения.
            double tug = 0.0;
            double acceleration = 0.0;
            double target_acceleration = 0.0;
            double speed = 0.0;
            double target_speed = 0.0;
            
            // Угловые величины.
            double heading_acceleration = 0.0;
            double target_heading = 0.0;
        };


    }; // namespace state_graph
    
}; // namespace xenon

