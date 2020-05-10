// *********************************************************************************************************************
// *                                           Main window of Xenon Desktop application                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 16 mar 2019 at 13:19 *
// *********************************************************************************************************************

// Qt includes
#include <QApplication>

// My own includes
#include "XeDesktop.h"
// #include ""

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Main program entry point                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

int main( int argc, char * argv[] ) {

    QApplication app( argc, argv );    
    xenon::XeDesktop * desktop = new xenon::XeDesktop();
    desktop->show();
    return app.exec();
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     The constructor                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

XeDesktop::XeDesktop( QWidget * parent )
    : QMainWindow( parent )
{
    
    // Не-визуальные элементы.
    __network = new XeNetwork(this);
    
    // Визуальные элементы.
    setupUi( this );
    __control = new ControlWidget(this->tab_control);
    this->control_layout->addWidget( __control );

    // Соединение элементов между собой.
    QObject::connect(__network, SIGNAL( signal__xplane_connected() ), this, SLOT( slot__xplane_connected() ) );
    QObject::connect(__network, SIGNAL( signal__xplane_disconnected() ), this, SLOT( slot__xplane_disconnected() ) );
    QObject::connect( __network, SIGNAL( signal__got_autopilot_state( Autopilot & )), __control, SLOT( slot__got_autopilot_state( Autopilot & )));

    // После того, как все создали - читаем и принимаем установки.
    __settings_restore();
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Main window close event                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void XeDesktop::closeEvent( QCloseEvent * event ) {
    
    __settings_store();
    QMainWindow::closeEvent( event );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Save settings at window close event                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void XeDesktop::__settings_store() {

    CREATE_XE_DESKTOP_SETTINGS;
    settings.beginGroup("MainWindow");
    settings.setValue("MainWindowGeometry", geometry() );
    settings.endGroup();
    settings.sync();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Restore settings when create MainWindow object                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void XeDesktop::__settings_restore() {
    
    CREATE_XE_DESKTOP_SETTINGS;

    settings.beginGroup("MainWindow");
    
    // Проверка вменяемости значений. Ширина-высота должны быть меньше, чем у экрана.
    // Записанные ширина-высота и x/y должны тоже быть более-менее похожими на правду.
    
    QRect rec = QApplication::desktop()->screenGeometry();    
    int screen_height = rec.height();
    int screen_width = rec.width();

    QRect mainWindowGeometry = settings.value("MainWindowGeometry", QRect(0,0,0,0) ).toRect();
    if (
        ( mainWindowGeometry.height() > 100 )
        && ( mainWindowGeometry.width() > 100 )
        && ( mainWindowGeometry.x() >= 0 )
        && ( mainWindowGeometry.y() >= 0 )
        && ( mainWindowGeometry.x() + mainWindowGeometry.width() < screen_width )
        && ( mainWindowGeometry.y() + mainWindowGeometry.height() < screen_height )
    ) {
        // MainWindow object settings have reasonable geometry.
        this->setGeometry( mainWindowGeometry );
    }

    settings.endGroup();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                            The X-Plane simulator has been "connected" to this software                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void XeDesktop::slot__xplane_connected() {
    qDebug() << "X-Plane has been connected";
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         The X-Plane simulator has been "disconnected" from this software                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void XeDesktop::slot__xplane_disconnected() {
    qDebug() << "X-Plane has been disconnected";
}

