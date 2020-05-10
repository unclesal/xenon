// *********************************************************************************************************************
// *                                Кнопка с возможностью повторения нажатия при ее удержании.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 17:18 *
// *********************************************************************************************************************

#include "RepeatableButton.h"

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Конструктор                                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

RepeatableButton::RepeatableButton( QWidget * parent )
    : QFrame( parent )
{
    __repeatable = false;
    __iconable = false;

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    setFrameStyle( QFrame::Panel | QFrame::Raised );
    setAutoFillBackground( true );

    setLineWidth( REPEATABLE_BUTTON_DISABLED_LINE_WIDTH );
    setMidLineWidth( REPEATABLE_BUTTON_DISABLED_LINE_WIDTH );

    // Внутренний менеджер расположения.

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);
    this->setLayout( layout );

    // Верхний виджит, для панельки с индикатором.
    QWidget * top_widget = new QWidget();
    top_widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    layout->addWidget( top_widget );

    QHBoxLayout * top_layout = new QHBoxLayout( top_widget );
    top_layout->setSpacing( 2 );
    top_layout->setMargin( 2 );
    top_layout->setContentsMargins(8, 8, 8, 0);
    top_widget->setLayout( top_layout );

    // Панелька с индикатором.
    inner_frame = new QFrame( top_widget );
    inner_frame->setFrameStyle( QFrame::NoFrame );
    inner_frame->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    inner_frame->setAutoFillBackground( true );
    inner_frame->setVisible( false );
    top_layout->addWidget( inner_frame );

    // Нижний виджит - метка.

    label = new QLabel( this );
    label->setText( "" );
    label->setAlignment( Qt::AlignCenter );
    __font.setFamily( "Tahoma" );
    __font.setPixelSize(18);
    __font.setBold( true );
    label->setFont( __font );

    QPalette pal( label->palette() );
    pal.setColor( QPalette::Foreground, REPEATABLE_BUTTON_ENABLED_LABEL_COLOR );
    label->setPalette( pal );

    layout->addWidget( label );

    // Пока нет соединения, виджит все равно запрещен будет.
    this->disable();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Событие изменения размера виджита.                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void RepeatableButton::resizeEvent( QResizeEvent * event ) {

    QFrame::resizeEvent( event );
    int h = event->size().height();
    int fh = qRound( h / 4.0 );
    __font.setPixelSize( fh );
    label->setFont( __font );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Разрешение виджита                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void RepeatableButton::enable() {

    setDisabled( false );

    setLineWidth( REPEATABLE_BUTTON_ENABLED_LINE_WIDTH );
    setMidLineWidth( REPEATABLE_BUTTON_ENABLED_LINE_WIDTH );

    QPalette rb_palette(palette());
    rb_palette.setColor( QPalette::Background, ENABLED_BACKGROUND );
    setPalette( rb_palette );

    QPalette pal( label->palette() );
    pal.setColor( QPalette::Foreground, REPEATABLE_BUTTON_ENABLED_LABEL_COLOR );
    label->setPalette( pal );

    QPalette inner_frame_palette ( inner_frame->palette() );
    inner_frame_palette.setColor( QPalette::Background, __indicator_color );
    inner_frame->setPalette( inner_frame_palette );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Запрещение виджита.                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************


void RepeatableButton::disable() {

    setDisabled( true );

    setLineWidth( REPEATABLE_BUTTON_DISABLED_LINE_WIDTH );
    setMidLineWidth( REPEATABLE_BUTTON_DISABLED_LINE_WIDTH );

    QPalette rb_palette(palette());
    rb_palette.setColor( QPalette::Background, DISABLED_BACKGROUND );
    setPalette( rb_palette );

    QPalette label_palette( label->palette() );
    label_palette.setColor( QPalette::Foreground, REPEATABLE_BUTTON_DISABLED_LABEL_COLOR );
    label->setPalette( label_palette );

    QPalette inner_frame_palette ( inner_frame->palette() );
    inner_frame_palette.setColor( QPalette::Background, REPEATABLE_BUTTON_DISABLED_INDICATOR_COLOR );
    inner_frame->setPalette( inner_frame_palette );
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Событие нажатия мышки на виджите.                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void RepeatableButton::mousePressEvent( QMouseEvent * event ) {
    QWidget::mousePressEvent( event );
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    emit signal__fired();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Событие отпускания мышки на виджите.                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void RepeatableButton::mouseReleaseEvent( QMouseEvent * event ) {
    QWidget::mouseReleaseEvent( event );
    setFrameStyle( QFrame::Panel | QFrame::Raised );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Установить серый (неактивный) цвет индикатора.                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void RepeatableButton::setColor( QColor indicator_color ) {
    __indicator_color = indicator_color;
    if (( __iconable ) && (isEnabled())) {
        QPalette pal ( inner_frame->palette() );
        pal.setColor( QPalette::Background, __indicator_color );
        inner_frame->setPalette( pal );
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                   Установка "иконофикации" кнопки, т.е. кнопка будет с индикатором состояния.                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void RepeatableButton::setIconable(bool iconable) {
     __iconable = iconable;
     if ( __iconable ) {
        inner_frame->setFrameStyle( QFrame::Panel | QFrame::Sunken );
        inner_frame->setVisible( true );
     } else {
        inner_frame->setFrameStyle( QFrame::NoFrame );
        inner_frame->setVisible( false );
     }
}
