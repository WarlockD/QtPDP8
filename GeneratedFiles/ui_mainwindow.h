/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "console.h"
#include "placabletextedit.h"
#include "qtty2.h"
#include "switchrow.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    SwitchRow *switchRow;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_6;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QPushButton *pushButton_5;
    QPushButton *pushButton_7;
    QPushButton *pushButton_6;
    QPushButton *pushButton_4;
    QPushButton *pushButton_3;
    QPushButton *pushButton_11;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *labelPC;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QLabel *labelMA;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *labelMB;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_4;
    QLabel *labelAC;
    QGroupBox *groupBox_7;
    QVBoxLayout *verticalLayout_9;
    QLabel *labelState;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_7;
    QLabel *labelMQ;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_5;
    QLabel *labelStepCounter;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    Console *debugConsole;
    QPushButton *pushButton_12;
    QPushButton *pushButton_13;
    QPushButton *pushButton_14;
    PlacableTextEdit *stupidTerm;
    QTTY2 *simpleConsole;
    QPlainTextEdit *dsamEdit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1550, 909);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setAutoFillBackground(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        switchRow = new SwitchRow(centralWidget);
        switchRow->setObjectName(QStringLiteral("switchRow"));
        switchRow->setGeometry(QRect(40, 410, 580, 81));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(switchRow->sizePolicy().hasHeightForWidth());
        switchRow->setSizePolicy(sizePolicy1);
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 90, 77, 228));
        verticalLayout_6 = new QVBoxLayout(layoutWidget);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        pushButton_2 = new QPushButton(layoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout_6->addWidget(pushButton_2);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout_6->addWidget(pushButton);

        pushButton_5 = new QPushButton(layoutWidget);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));

        verticalLayout_6->addWidget(pushButton_5);

        pushButton_7 = new QPushButton(layoutWidget);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));

        verticalLayout_6->addWidget(pushButton_7);

        pushButton_6 = new QPushButton(layoutWidget);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));

        verticalLayout_6->addWidget(pushButton_6);

        pushButton_4 = new QPushButton(layoutWidget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        verticalLayout_6->addWidget(pushButton_4);

        pushButton_3 = new QPushButton(layoutWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        verticalLayout_6->addWidget(pushButton_3);

        pushButton_11 = new QPushButton(layoutWidget);
        pushButton_11->setObjectName(QStringLiteral("pushButton_11"));

        verticalLayout_6->addWidget(pushButton_11);

        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(150, 80, 100, 311));
        verticalLayout_8 = new QVBoxLayout(layoutWidget1);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        groupBox_3 = new QGroupBox(layoutWidget1);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        sizePolicy1.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy1);
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
        font.setBold(false);
        font.setWeight(50);
        groupBox_3->setFont(font);
        groupBox_3->setAlignment(Qt::AlignCenter);
        groupBox_3->setFlat(true);
        groupBox_3->setCheckable(false);
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        labelPC = new QLabel(groupBox_3);
        labelPC->setObjectName(QStringLiteral("labelPC"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(labelPC->sizePolicy().hasHeightForWidth());
        labelPC->setSizePolicy(sizePolicy2);
        QFont font1;
        font1.setFamily(QStringLiteral("Courier"));
        font1.setPointSize(14);
        font1.setBold(true);
        font1.setWeight(75);
        labelPC->setFont(font1);
        labelPC->setLayoutDirection(Qt::LeftToRight);
        labelPC->setTextFormat(Qt::PlainText);
        labelPC->setScaledContents(false);
        labelPC->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(labelPC);


        verticalLayout_8->addWidget(groupBox_3);

        groupBox = new QGroupBox(layoutWidget1);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        groupBox->setFont(font);
        groupBox->setAlignment(Qt::AlignCenter);
        groupBox->setFlat(true);
        groupBox->setCheckable(false);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        labelMA = new QLabel(groupBox);
        labelMA->setObjectName(QStringLiteral("labelMA"));
        sizePolicy2.setHeightForWidth(labelMA->sizePolicy().hasHeightForWidth());
        labelMA->setSizePolicy(sizePolicy2);
        labelMA->setFont(font1);
        labelMA->setLayoutDirection(Qt::LeftToRight);
        labelMA->setTextFormat(Qt::PlainText);
        labelMA->setScaledContents(false);
        labelMA->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelMA);


        verticalLayout_8->addWidget(groupBox);

        groupBox_2 = new QGroupBox(layoutWidget1);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        groupBox_2->setFont(font);
        groupBox_2->setAlignment(Qt::AlignCenter);
        groupBox_2->setFlat(true);
        groupBox_2->setCheckable(false);
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        labelMB = new QLabel(groupBox_2);
        labelMB->setObjectName(QStringLiteral("labelMB"));
        sizePolicy2.setHeightForWidth(labelMB->sizePolicy().hasHeightForWidth());
        labelMB->setSizePolicy(sizePolicy2);
        labelMB->setFont(font1);
        labelMB->setLayoutDirection(Qt::LeftToRight);
        labelMB->setTextFormat(Qt::PlainText);
        labelMB->setScaledContents(false);
        labelMB->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(labelMB);


        verticalLayout_8->addWidget(groupBox_2);

        groupBox_4 = new QGroupBox(layoutWidget1);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        sizePolicy1.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy1);
        groupBox_4->setFont(font);
        groupBox_4->setAlignment(Qt::AlignCenter);
        groupBox_4->setFlat(true);
        groupBox_4->setCheckable(false);
        verticalLayout_4 = new QVBoxLayout(groupBox_4);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        labelAC = new QLabel(groupBox_4);
        labelAC->setObjectName(QStringLiteral("labelAC"));
        sizePolicy2.setHeightForWidth(labelAC->sizePolicy().hasHeightForWidth());
        labelAC->setSizePolicy(sizePolicy2);
        labelAC->setFont(font1);
        labelAC->setLayoutDirection(Qt::LeftToRight);
        labelAC->setTextFormat(Qt::PlainText);
        labelAC->setScaledContents(false);
        labelAC->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(labelAC);


        verticalLayout_8->addWidget(groupBox_4);

        groupBox_7 = new QGroupBox(layoutWidget1);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        sizePolicy1.setHeightForWidth(groupBox_7->sizePolicy().hasHeightForWidth());
        groupBox_7->setSizePolicy(sizePolicy1);
        groupBox_7->setFont(font);
        groupBox_7->setAlignment(Qt::AlignCenter);
        groupBox_7->setFlat(true);
        groupBox_7->setCheckable(false);
        verticalLayout_9 = new QVBoxLayout(groupBox_7);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        labelState = new QLabel(groupBox_7);
        labelState->setObjectName(QStringLiteral("labelState"));
        sizePolicy2.setHeightForWidth(labelState->sizePolicy().hasHeightForWidth());
        labelState->setSizePolicy(sizePolicy2);
        labelState->setFont(font1);
        labelState->setLayoutDirection(Qt::LeftToRight);
        labelState->setTextFormat(Qt::PlainText);
        labelState->setScaledContents(false);
        labelState->setAlignment(Qt::AlignCenter);

        verticalLayout_9->addWidget(labelState);


        verticalLayout_8->addWidget(groupBox_7);

        groupBox_6 = new QGroupBox(layoutWidget1);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        sizePolicy1.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy1);
        groupBox_6->setFont(font);
        groupBox_6->setAlignment(Qt::AlignCenter);
        groupBox_6->setFlat(true);
        groupBox_6->setCheckable(false);
        verticalLayout_7 = new QVBoxLayout(groupBox_6);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        labelMQ = new QLabel(groupBox_6);
        labelMQ->setObjectName(QStringLiteral("labelMQ"));
        sizePolicy2.setHeightForWidth(labelMQ->sizePolicy().hasHeightForWidth());
        labelMQ->setSizePolicy(sizePolicy2);
        labelMQ->setFont(font1);
        labelMQ->setLayoutDirection(Qt::LeftToRight);
        labelMQ->setTextFormat(Qt::PlainText);
        labelMQ->setScaledContents(false);
        labelMQ->setAlignment(Qt::AlignCenter);

        verticalLayout_7->addWidget(labelMQ);


        verticalLayout_8->addWidget(groupBox_6);

        groupBox_5 = new QGroupBox(layoutWidget1);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        sizePolicy1.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy1);
        groupBox_5->setFont(font);
        groupBox_5->setAlignment(Qt::AlignCenter);
        groupBox_5->setFlat(true);
        groupBox_5->setCheckable(false);
        verticalLayout_5 = new QVBoxLayout(groupBox_5);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        labelStepCounter = new QLabel(groupBox_5);
        labelStepCounter->setObjectName(QStringLiteral("labelStepCounter"));
        sizePolicy2.setHeightForWidth(labelStepCounter->sizePolicy().hasHeightForWidth());
        labelStepCounter->setSizePolicy(sizePolicy2);
        labelStepCounter->setFont(font1);
        labelStepCounter->setLayoutDirection(Qt::LeftToRight);
        labelStepCounter->setTextFormat(Qt::PlainText);
        labelStepCounter->setScaledContents(false);
        labelStepCounter->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(labelStepCounter);


        verticalLayout_8->addWidget(groupBox_5);

        pushButton_8 = new QPushButton(centralWidget);
        pushButton_8->setObjectName(QStringLiteral("pushButton_8"));
        pushButton_8->setGeometry(QRect(60, 30, 75, 23));
        pushButton_9 = new QPushButton(centralWidget);
        pushButton_9->setObjectName(QStringLiteral("pushButton_9"));
        pushButton_9->setGeometry(QRect(140, 30, 75, 23));
        pushButton_10 = new QPushButton(centralWidget);
        pushButton_10->setObjectName(QStringLiteral("pushButton_10"));
        pushButton_10->setGeometry(QRect(220, 30, 75, 23));
        debugConsole = new Console(centralWidget);
        debugConsole->setObjectName(QStringLiteral("debugConsole"));
        debugConsole->setGeometry(QRect(640, 240, 271, 101));
        QFont font2;
        font2.setFamily(QStringLiteral("Courier New"));
        debugConsole->setFont(font2);
        pushButton_12 = new QPushButton(centralWidget);
        pushButton_12->setObjectName(QStringLiteral("pushButton_12"));
        pushButton_12->setGeometry(QRect(300, 30, 75, 23));
        pushButton_13 = new QPushButton(centralWidget);
        pushButton_13->setObjectName(QStringLiteral("pushButton_13"));
        pushButton_13->setGeometry(QRect(380, 30, 75, 23));
        pushButton_14 = new QPushButton(centralWidget);
        pushButton_14->setObjectName(QStringLiteral("pushButton_14"));
        pushButton_14->setGeometry(QRect(460, 30, 75, 23));
        stupidTerm = new PlacableTextEdit(centralWidget);
        stupidTerm->setObjectName(QStringLiteral("stupidTerm"));
        stupidTerm->setGeometry(QRect(640, 350, 881, 501));
        simpleConsole = new QTTY2(centralWidget);
        simpleConsole->setObjectName(QStringLiteral("simpleConsole"));
        simpleConsole->setGeometry(QRect(320, 70, 301, 281));
        dsamEdit = new QPlainTextEdit(centralWidget);
        dsamEdit->setObjectName(QStringLiteral("dsamEdit"));
        dsamEdit->setGeometry(QRect(633, 50, 641, 181));
        dsamEdit->setFont(font2);
        dsamEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        dsamEdit->setReadOnly(true);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1550, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        pushButton_2->setText(QApplication::translate("MainWindow", "Load Add", 0));
        pushButton->setText(QApplication::translate("MainWindow", "Start", 0));
        pushButton_5->setText(QApplication::translate("MainWindow", "Cont", 0));
        pushButton_7->setText(QApplication::translate("MainWindow", "Stop", 0));
        pushButton_6->setText(QApplication::translate("MainWindow", "Step", 0));
        pushButton_4->setText(QApplication::translate("MainWindow", "Exam", 0));
        pushButton_3->setText(QApplication::translate("MainWindow", "Dep", 0));
        pushButton_11->setText(QApplication::translate("MainWindow", "Clear", 0));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Program Counter", 0));
        labelPC->setText(QApplication::translate("MainWindow", "0000", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "Memory Address", 0));
        labelMA->setText(QApplication::translate("MainWindow", "0000", 0));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Memory Buffer", 0));
        labelMB->setText(QApplication::translate("MainWindow", "0000", 0));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Accumulator", 0));
        labelAC->setText(QApplication::translate("MainWindow", "0000", 0));
        groupBox_7->setTitle(QApplication::translate("MainWindow", "State", 0));
        labelState->setText(QApplication::translate("MainWindow", "Fetch", 0));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "Multipier Quotient", 0));
        labelMQ->setText(QApplication::translate("MainWindow", "0000", 0));
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Step Counter", 0));
        labelStepCounter->setText(QApplication::translate("MainWindow", "0000", 0));
        pushButton_8->setText(QApplication::translate("MainWindow", "Inst1", 0));
        pushButton_9->setText(QApplication::translate("MainWindow", "Inst2", 0));
        pushButton_10->setText(QApplication::translate("MainWindow", "Adder", 0));
        pushButton_12->setText(QApplication::translate("MainWindow", "tty", 0));
        pushButton_13->setText(QApplication::translate("MainWindow", "vt05", 0));
        pushButton_14->setText(QApplication::translate("MainWindow", "focal", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
