// *********************************************************************************************************************
// *                                           Main window of Xenon Desktop application                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 16 mar 2019 at 13:17 *
// *********************************************************************************************************************

#pragma once

// Qt includes

#include <QWidget>
#include <QMainWindow>
#include <QSettings>
#include <QEvent>
#include <QDesktopWidget>

#define XE_DESKTOP_APPLICATION_NAME         "XeDesktop"
#define CREATE_XE_DESKTOP_SETTINGS           QSettings settings( XENON_ORGANIZATION_NAME, XE_DESKTOP_APPLICATION_NAME )

// My own includes.
#include "Settings.h"
#include "XeNetwork.h"
#include "ControlWidget.h"

#include "ui_main_window.h"

namespace xenon {
    
    class XeDesktop : public QMainWindow, public Ui::MainWindow {
        
        Q_OBJECT
        public:
            
            XeDesktop( QWidget * parent = nullptr );
            virtual ~XeDesktop() override = default;
            
        protected:
            
            void closeEvent( QCloseEvent * event ) override;            
            
        private:
            
            XeNetwork * __network;
            ControlWidget * __control;
            
            void __settings_store();
            void __settings_restore();
            
        private slots:
            
            void slot__xplane_connected();
            void slot__xplane_disconnected();
        
    };
    
}; // namespace xenon
