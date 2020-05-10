// *********************************************************************************************************************
// *                                            Виджит вертикальной скорости.                                          *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 19:25 *
// *********************************************************************************************************************

#pragma once
#include "ui_vertical_speed.h"

class VerticalSpeed: public QWidget, public Ui::VerticalSpeed {
    Q_OBJECT
    public:

        VerticalSpeed( QWidget * parent = nullptr);
        virtual ~VerticalSpeed() override = default;

    protected:

    private:
};
