// *********************************************************************************************************************
// *                                    Виджит индикации и управления курсом самолета.                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 17:03 *
// *********************************************************************************************************************

#pragma once

#include "ui_heading.h"
#include "RepeatableButton.h"

class Heading: public QWidget, public Ui::Heading {

    Q_OBJECT
    public:

        Heading( QWidget * parent = nullptr );
        virtual ~Heading() override = default;

        RepeatableButton * button_left;
        RepeatableButton * button_right;
        RepeatableButton * button_center;

    protected:

    private:
};
