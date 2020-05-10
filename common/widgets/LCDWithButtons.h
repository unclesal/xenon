// *********************************************************************************************************************
// *                                         Виджит с LCD индикатором и кнопками снизу.                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 16 sep 2019 at 12:35 *
// *********************************************************************************************************************

#pragma once

#include <QWidget>
#include <QPalette>
#include <QColor>

#include "ui_lcd_with_buttons.h"
#include "RepeatableButton.h"

class LCDWithButtons : public QWidget, public Ui::LCDWithButtons {

    Q_OBJECT

    public:

        LCDWithButtons( QWidget * parent = nullptr );
        virtual ~LCDWithButtons() override = default;

        RepeatableButton * button_down;
        RepeatableButton * button_center;
        RepeatableButton * button_up;

        void setColor( QColor indicator_color );

        void enable();
        void disable();

        void setStep( int step ) { __step = step; }
        int step() { return __step; }

    protected:
    private:

        QColor __indicator_color;
        int __step;
};

