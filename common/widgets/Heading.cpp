// *********************************************************************************************************************
// *                                    Виджит индикации и управления курсом самолета.                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 17:06 *
// *********************************************************************************************************************

#include "Heading.h"

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

Heading::Heading( QWidget * parent )
    : QWidget( parent )
{
    setupUi( this );
    lcd->display("000");

    button_left = new RepeatableButton( this );
    // У левой кнопки нет индикатора, там просто уменьшение курса.
    button_left->internal_frame->setFrameStyle( QFrame::NoFrame );
    this->layout_left->addWidget( button_left );

    button_right = new RepeatableButton( this );
    // У правой кнопки нет индикатора, там просто увеличение курса.
    button_right->internal_frame->setFrameStyle( QFrame::NoFrame );
    this->layout_right->addWidget( button_right );

    button_center = new RepeatableButton( this );
    this->layout_center->addWidget( button_center );
}
