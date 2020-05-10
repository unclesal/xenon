// *********************************************************************************************************************
// *                                   Виджит индикации и установки высоты автопилота.                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 19:54 *
// *********************************************************************************************************************

#pragma once

#include <QWidget>

#include "ui_altitude.h"

class Altitude: public QWidget, public Ui::Altitude {

    Q_OBJECT
    public:

        Altitude( QWidget * parent = nullptr );
        virtual ~Altitude() override = default;

    protected:
    private:
};
