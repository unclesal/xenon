// *********************************************************************************************************************
// *                                    Отдельная реализация виджита управления автопилотом.                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 20 sep 2019 at 19:00 *
// *********************************************************************************************************************

#include <QApplication>

#include "XeAutopilot.h"

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                      M A I N                                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

int main( int argc, char * argv[] ) {

    QApplication a(argc, argv);
    XeAutopilot * ap = new XeAutopilot();
    ap->show();
    return a.exec();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

XeAutopilot::XeAutopilot( QWidget * parent )
    : QWidget( parent )
{

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    QHBoxLayout * lay = new QHBoxLayout( this );
    lay->setMargin( 0 );
    lay->setSpacing( 0 );
    setLayout( lay );

    __ap_widget = new AutopilotControlWidget(this);
    lay->addWidget( __ap_widget );

#ifdef UX_DESKTOP
    setFixedSize(QSize(1100, 600));
#endif

    // Сеть.
    __network = new XeNetwork();

    // Соединение сигналов-слотов.
    QObject::connect( __network, SIGNAL( signal__xplane_connected() ), __ap_widget, SLOT( slot__xplane_connected() ) );
    QObject::connect( __network, SIGNAL( signal__xplane_disconnected() ), __ap_widget, SLOT( slot__xplane_disconnected() ));
    QObject::connect( __network, SIGNAL( signal__got_autopilot_state( CommandAutopilotState & )), __ap_widget, SLOT( slot__got_autopilot_state( CommandAutopilotState & ) ));
    QObject::connect( __network, SIGNAL( signal__got_user_aircraft_state( AircraftState & ) ), __ap_widget, SLOT( slot__got_user_aircraft_state( AircraftState & )) );

    QObject::connect( __ap_widget, SIGNAL( signal__need_send( CommandSet & )), __network, SLOT( slot__need_send( CommandSet & )));

}
