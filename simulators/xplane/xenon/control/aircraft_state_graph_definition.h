// *********************************************************************************************************************
// *                                          Определения для графа состояний самолета                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 10:56 *
// *********************************************************************************************************************

#pragma once

#include <boost/graph/adjacency_list.hpp>

#include "structures.h"

namespace xenon {
    
    namespace aircraft_state_graph {
        
        // Состояния (узлы графа).
        enum aircraft_state_t {
            ACF_STATE_UNKNOWN = 0,
            ACF_STATE_PARKING,
            ACF_STATE_READY_FOR_TAXING,
            // Находится на исполнительном старте.
            ACF_STATE_HP,
            ACF_STATE_READY_FOR_TAKEOF
        };

        // Действия (ребра графа).
        enum aircraft_does_t {
            ACF_DOES_NOTHING = 0,
            // Выталкивается.
            ACF_DOES_PUSH_BACK,
            // Рулежка.
            ACF_DOES_SLOW_TAXING,
            ACF_DOES_NORMAL_TAXING,
            // Останавливается при (после) рулежке, хорошо так прямо
            // тормозит вплоть до полной его остановки.
            ACF_DOES_TAXING_STOP,

            // Скорее всего - стоит на HP. Но не факт,
            // разрешение на взлет может быть получено сразу.
            ACF_DOES_WAITING_TAKE_OFF_APPROVAL,
            ACF_DOES_TAKE_OFF,
            ACF_DOES_CLIMBING,
            ACF_DOES_CRUISING,
            ACF_DOES_DESCENDING
        };

        
        // Узел графа - состояние самолета.
        struct node_t {
            // Описание состояния
            aircraft_state_t state = ACF_STATE_UNKNOWN;
            // Указатель на класс состояния, AbstractState.
            // Но его тип здесь специфицировать нельзя, иначе
            // получим циклическую ссылку инклудников.
            void * ptr_state_class = nullptr;
            bool current_state = false;
        };

        // Ребро графа - действие для перехода в другое состояние.
        struct edge_t {
            aircraft_does_t does = ACF_DOES_NOTHING;
            // Указатель на класс действия.
            void * ptr_does_class = nullptr;
            bool current_does = false;
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

    }; // namespace state_graph
    
}; // namespace xenon

