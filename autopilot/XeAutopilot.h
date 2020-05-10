// *********************************************************************************************************************
// *                                    Отдельная реализация виджита управления автопилотом.                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 20 sep 2019 at 18:58 *
// *********************************************************************************************************************
#pragma once

#include <QWidget>
#include <QHBoxLayout>

#include "AutopilotControlWidget.h"

#include "CommandAutopilotState.h"
#include "CommandSet.h"
#include "XeNetwork.h"

using namespace xenon;

class XeAutopilot : public QWidget {

    Q_OBJECT

    public:

        XeAutopilot( QWidget * parent = nullptr );
        virtual ~XeAutopilot() override = default;

    protected:
    private:

        AutopilotControlWidget * __ap_widget;
        XeNetwork * __network;

};
