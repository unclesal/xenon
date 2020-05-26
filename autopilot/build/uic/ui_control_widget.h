/********************************************************************************
** Form generated from reading UI file 'control_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROL_WIDGET_H
#define UI_CONTROL_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *top_layout;
    QWidget *speed_widget;
    QVBoxLayout *verticalLayout_7;
    QVBoxLayout *speed_layout;
    QWidget *altitude_widget;
    QVBoxLayout *verticalLayout_8;
    QVBoxLayout *altitude_layout;
    QHBoxLayout *bottom_layout;
    QWidget *heading_widget;
    QVBoxLayout *verticalLayout_10;
    QVBoxLayout *heading_layout;
    QWidget *vertical_velocity_widget;
    QVBoxLayout *verticalLayout_9;
    QVBoxLayout *vertical_velocity_layout;
    QVBoxLayout *right_layout;

    void setupUi(QWidget *ControlWidget)
    {
        if (ControlWidget->objectName().isEmpty())
            ControlWidget->setObjectName(QString::fromUtf8("ControlWidget"));
        ControlWidget->resize(1094, 601);
        horizontalLayout = new QHBoxLayout(ControlWidget);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 15);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(8);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        top_layout = new QHBoxLayout();
        top_layout->setSpacing(8);
        top_layout->setObjectName(QString::fromUtf8("top_layout"));
        speed_widget = new QWidget(ControlWidget);
        speed_widget->setObjectName(QString::fromUtf8("speed_widget"));
        verticalLayout_7 = new QVBoxLayout(speed_widget);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        speed_layout = new QVBoxLayout();
        speed_layout->setSpacing(0);
        speed_layout->setObjectName(QString::fromUtf8("speed_layout"));

        verticalLayout_7->addLayout(speed_layout);


        top_layout->addWidget(speed_widget);

        altitude_widget = new QWidget(ControlWidget);
        altitude_widget->setObjectName(QString::fromUtf8("altitude_widget"));
        verticalLayout_8 = new QVBoxLayout(altitude_widget);
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        altitude_layout = new QVBoxLayout();
        altitude_layout->setSpacing(0);
        altitude_layout->setObjectName(QString::fromUtf8("altitude_layout"));

        verticalLayout_8->addLayout(altitude_layout);


        top_layout->addWidget(altitude_widget);


        verticalLayout->addLayout(top_layout);

        bottom_layout = new QHBoxLayout();
        bottom_layout->setSpacing(8);
        bottom_layout->setObjectName(QString::fromUtf8("bottom_layout"));
        heading_widget = new QWidget(ControlWidget);
        heading_widget->setObjectName(QString::fromUtf8("heading_widget"));
        verticalLayout_10 = new QVBoxLayout(heading_widget);
        verticalLayout_10->setSpacing(0);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(0, 0, 0, 0);
        heading_layout = new QVBoxLayout();
        heading_layout->setSpacing(0);
        heading_layout->setObjectName(QString::fromUtf8("heading_layout"));

        verticalLayout_10->addLayout(heading_layout);


        bottom_layout->addWidget(heading_widget);

        vertical_velocity_widget = new QWidget(ControlWidget);
        vertical_velocity_widget->setObjectName(QString::fromUtf8("vertical_velocity_widget"));
        verticalLayout_9 = new QVBoxLayout(vertical_velocity_widget);
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        vertical_velocity_layout = new QVBoxLayout();
        vertical_velocity_layout->setSpacing(0);
        vertical_velocity_layout->setObjectName(QString::fromUtf8("vertical_velocity_layout"));

        verticalLayout_9->addLayout(vertical_velocity_layout);


        bottom_layout->addWidget(vertical_velocity_widget);


        verticalLayout->addLayout(bottom_layout);


        horizontalLayout->addLayout(verticalLayout);

        right_layout = new QVBoxLayout();
        right_layout->setSpacing(0);
        right_layout->setObjectName(QString::fromUtf8("right_layout"));
        right_layout->setContentsMargins(0, 0, 0, 0);

        horizontalLayout->addLayout(right_layout);

        horizontalLayout->setStretch(0, 10);
        horizontalLayout->setStretch(1, 3);

        retranslateUi(ControlWidget);

        QMetaObject::connectSlotsByName(ControlWidget);
    } // setupUi

    void retranslateUi(QWidget *ControlWidget)
    {
        ControlWidget->setWindowTitle(QCoreApplication::translate("ControlWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ControlWidget: public Ui_ControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROL_WIDGET_H
