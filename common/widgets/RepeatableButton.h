// *********************************************************************************************************************
// *                                Кнопка с возможностью повторения нажатия при ее удержании.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 17:03 *
// *********************************************************************************************************************

#pragma once

#include <QFrame>
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QFont>
#include <QColor>
#include <QPalette>
#include <QVBoxLayout>

#include "UserInterface.h"

class RepeatableButton: public QFrame {

    Q_OBJECT

    signals:

        void signal__fired();

    public:

        RepeatableButton( QWidget * parent = nullptr );
        virtual ~RepeatableButton() override = default;

        void setRepeatable(bool repeatable) { __repeatable = repeatable; }
        bool repeatable() { return __repeatable; }

        void setIconable(bool iconable);
        bool iconable() { return __iconable; }

        QLabel * label;
        QFrame * inner_frame;

        void setColor(QColor indicator_color);

        void enable();
        void disable();

    protected:

        void mousePressEvent(QMouseEvent * event) override;
        void mouseReleaseEvent(QMouseEvent * event) override;
        void resizeEvent(QResizeEvent * event ) override;

    private:

        bool __repeatable;
        bool __iconable;

        QFont __font;

        QColor __indicator_color;

};
