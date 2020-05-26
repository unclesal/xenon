/********************************************************************************
** Form generated from reading UI file 'lcd_with_buttons.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LCD_WITH_BUTTONS_H
#define UI_LCD_WITH_BUTTONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LCDWithButtons
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLCDNumber *lcd;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QWidget *widget_button_left;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *layout_left;
    QWidget *widget_button_center;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *layout_center;
    QWidget *widget_button_right;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *layout_right;

    void setupUi(QWidget *LCDWithButtons)
    {
        if (LCDWithButtons->objectName().isEmpty())
            LCDWithButtons->setObjectName(QString::fromUtf8("LCDWithButtons"));
        LCDWithButtons->resize(746, 386);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LCDWithButtons->sizePolicy().hasHeightForWidth());
        LCDWithButtons->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(LCDWithButtons);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(4);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lcd = new QLCDNumber(LCDWithButtons);
        lcd->setObjectName(QString::fromUtf8("lcd"));
        sizePolicy.setHeightForWidth(lcd->sizePolicy().hasHeightForWidth());
        lcd->setSizePolicy(sizePolicy);
        lcd->setFrameShape(QFrame::StyledPanel);
        lcd->setFrameShadow(QFrame::Sunken);
        lcd->setSmallDecimalPoint(false);
        lcd->setDigitCount(5);
        lcd->setSegmentStyle(QLCDNumber::Filled);
        lcd->setProperty("value", QVariant(88888.000000000000000));
        lcd->setProperty("intValue", QVariant(88888));

        verticalLayout->addWidget(lcd);

        widget = new QWidget(LCDWithButtons);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        widget_button_left = new QWidget(widget);
        widget_button_left->setObjectName(QString::fromUtf8("widget_button_left"));
        horizontalLayout_3 = new QHBoxLayout(widget_button_left);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        layout_left = new QVBoxLayout();
        layout_left->setSpacing(0);
        layout_left->setObjectName(QString::fromUtf8("layout_left"));

        horizontalLayout_3->addLayout(layout_left);


        horizontalLayout->addWidget(widget_button_left);

        widget_button_center = new QWidget(widget);
        widget_button_center->setObjectName(QString::fromUtf8("widget_button_center"));
        horizontalLayout_4 = new QHBoxLayout(widget_button_center);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        layout_center = new QVBoxLayout();
        layout_center->setSpacing(0);
        layout_center->setObjectName(QString::fromUtf8("layout_center"));

        horizontalLayout_4->addLayout(layout_center);


        horizontalLayout->addWidget(widget_button_center);

        widget_button_right = new QWidget(widget);
        widget_button_right->setObjectName(QString::fromUtf8("widget_button_right"));
        horizontalLayout_5 = new QHBoxLayout(widget_button_right);
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        layout_right = new QVBoxLayout();
        layout_right->setSpacing(0);
        layout_right->setObjectName(QString::fromUtf8("layout_right"));

        horizontalLayout_5->addLayout(layout_right);


        horizontalLayout->addWidget(widget_button_right);


        horizontalLayout_2->addLayout(horizontalLayout);


        verticalLayout->addWidget(widget);

        verticalLayout->setStretch(0, 3);
        verticalLayout->setStretch(1, 1);

        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(LCDWithButtons);

        QMetaObject::connectSlotsByName(LCDWithButtons);
    } // setupUi

    void retranslateUi(QWidget *LCDWithButtons)
    {
        LCDWithButtons->setWindowTitle(QCoreApplication::translate("LCDWithButtons", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LCDWithButtons: public Ui_LCDWithButtons {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LCD_WITH_BUTTONS_H
