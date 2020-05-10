// *********************************************************************************************************************
// *                                         Виджит с LCD индикатором и кнопками снизу.                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 16 sep 2019 at 12:40 *
// *********************************************************************************************************************

#include "LCDWithButtons.h"

// Цвет фона индикатора. Фиксированный, типа "темный индиго".
// QColor LCDWithButtons::BACKGROUND_COLOR =  QColor(20, 41, 45);
// Серенький цвет индикаторов, неактивный индикатор.
// QColor LCDWithButtons::GRAY_COLOR = QColor(92, 92, 92 );

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор.                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

LCDWithButtons::LCDWithButtons( QWidget * parent )
    : QWidget( parent )
{
    setupUi( this );
    setAutoFillBackground( true );

    button_down = new RepeatableButton( this );
    layout_left->addWidget( button_down );

    button_center = new RepeatableButton( this );
    layout_center->addWidget( button_center );

    button_up = new RepeatableButton( this );
    layout_right->addWidget( button_up );

    lcd->setAutoFillBackground( true );
    setColor( LCD_ENABLED_GRAY_COLOR );

    __step = 50;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Разрешенность работы виджита                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void LCDWithButtons::enable() {

    button_down->enable();
    button_center->enable();
    button_up->enable();

    QPalette widget_palette( palette() );
    widget_palette.setColor( QPalette::Background, ENABLED_BACKGROUND );
    widget_palette.setColor( QPalette::Foreground, ENABLED_COLOR );
    setPalette( widget_palette );

    QPalette lcd_palette( lcd->palette() );
    lcd_palette.setColor( QPalette::Background, LCD_ENABLED_BACKGROUND );
    lcd_palette.setColor( QPalette::Foreground, __indicator_color );
    lcd->setPalette( lcd_palette );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Запрещение работы виджита                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void LCDWithButtons::disable() {

    button_down->disable();
    button_center->disable();
    button_up->disable();

    QPalette widget_palette( palette() );
    widget_palette.setColor( QPalette::Background, DISABLED_BACKGROUND );
    widget_palette.setColor( QPalette::Foreground, DISABLED_COLOR );
    setPalette( widget_palette );

    QPalette lcd_palette( lcd->palette() );
    lcd_palette.setColor( QPalette::Background, DISABLED_BACKGROUND );
    lcd_palette.setColor( QPalette::Foreground, DISABLED_COLOR );
    lcd->setPalette( lcd_palette );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Установить цвет цифр.                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

void LCDWithButtons::setColor( QColor indicator_color ) {
    __indicator_color = indicator_color;
    if ( isEnabled() ) {
        QPalette lcd_palette( lcd->palette() );
        lcd_palette.setColor( QPalette::Foreground, __indicator_color );
        lcd->setPalette( lcd_palette );
    }
}
