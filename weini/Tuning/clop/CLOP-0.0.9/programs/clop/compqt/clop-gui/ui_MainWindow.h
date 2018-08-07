/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "CPlotWidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Open;
    QAction *action_About;
    QAction *action_Quit;
    QAction *action_Start;
    QAction *action_SoftStop;
    QAction *action_HardStop;
    QAction *action_Close;
    QAction *action_About_Qt;
    QAction *action_Automatic_GUI_update;
    QAction *action_Update_GUI_manually_now;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabWidget;
    QWidget *tabExperiment;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QTextEdit *textEdit;
    QWidget *tabLog;
    QHBoxLayout *horizontalLayout_4;
    QListWidget *listWidget;
    QWidget *tabPlot;
    QHBoxLayout *horizontalLayout_7;
    CPlotWidget *plotWidget;
    QVBoxLayout *verticalLayout_5;
    QGridLayout *gridLayout;
    QLabel *label_6;
    QComboBox *comboX;
    QLabel *label_7;
    QComboBox *comboY;
    QLabel *label_8;
    QSpinBox *pointSizeSpinBox;
    QSlider *pointSizeSlider;
    QLabel *label_10;
    QSpinBox *firstSpinBox;
    QSlider *firstSlider;
    QLabel *label_9;
    QSpinBox *lastSpinBox;
    QSlider *lastSlider;
    QCheckBox *sampleWeightCheckBox;
    QSpacerItem *verticalSpacer;
    QCheckBox *mergeReplicationsCheckBox;
    QWidget *tabMatrix;
    QHBoxLayout *horizontalLayout_5;
    QTableWidget *matrixTable;
    QWidget *tabEigen;
    QHBoxLayout *horizontalLayout_6;
    QTableWidget *eigenTable;
    QWidget *tabMax;
    QHBoxLayout *horizontalLayout_8;
    QTableWidget *maxTable;
    QWidget *tabWinRate;
    QHBoxLayout *horizontalLayout_9;
    QTableWidget *winRateTable;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menu_Help;
    QMenu *menu_Experiment;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(673, 568);
        action_Open = new QAction(MainWindow);
        action_Open->setObjectName(QStringLiteral("action_Open"));
        action_About = new QAction(MainWindow);
        action_About->setObjectName(QStringLiteral("action_About"));
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QStringLiteral("action_Quit"));
        action_Start = new QAction(MainWindow);
        action_Start->setObjectName(QStringLiteral("action_Start"));
        action_SoftStop = new QAction(MainWindow);
        action_SoftStop->setObjectName(QStringLiteral("action_SoftStop"));
        action_HardStop = new QAction(MainWindow);
        action_HardStop->setObjectName(QStringLiteral("action_HardStop"));
        action_Close = new QAction(MainWindow);
        action_Close->setObjectName(QStringLiteral("action_Close"));
        action_About_Qt = new QAction(MainWindow);
        action_About_Qt->setObjectName(QStringLiteral("action_About_Qt"));
        action_Automatic_GUI_update = new QAction(MainWindow);
        action_Automatic_GUI_update->setObjectName(QStringLiteral("action_Automatic_GUI_update"));
        action_Automatic_GUI_update->setCheckable(true);
        action_Automatic_GUI_update->setChecked(true);
        action_Update_GUI_manually_now = new QAction(MainWindow);
        action_Update_GUI_manually_now->setObjectName(QStringLiteral("action_Update_GUI_manually_now"));
        action_Update_GUI_manually_now->setEnabled(false);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabExperiment = new QWidget();
        tabExperiment->setObjectName(QStringLiteral("tabExperiment"));
        horizontalLayout_3 = new QHBoxLayout(tabExperiment);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        textEdit = new QTextEdit(tabExperiment);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setLineWrapMode(QTextEdit::NoWrap);
        textEdit->setReadOnly(true);

        horizontalLayout_2->addWidget(textEdit);


        horizontalLayout_3->addLayout(horizontalLayout_2);

        tabWidget->addTab(tabExperiment, QString());
        tabLog = new QWidget();
        tabLog->setObjectName(QStringLiteral("tabLog"));
        horizontalLayout_4 = new QHBoxLayout(tabLog);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        listWidget = new QListWidget(tabLog);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        horizontalLayout_4->addWidget(listWidget);

        tabWidget->addTab(tabLog, QString());
        tabPlot = new QWidget();
        tabPlot->setObjectName(QStringLiteral("tabPlot"));
        horizontalLayout_7 = new QHBoxLayout(tabPlot);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        plotWidget = new CPlotWidget(tabPlot);
        plotWidget->setObjectName(QStringLiteral("plotWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plotWidget->sizePolicy().hasHeightForWidth());
        plotWidget->setSizePolicy(sizePolicy);
        plotWidget->setMouseTracking(true);

        horizontalLayout_7->addWidget(plotWidget);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_6 = new QLabel(tabPlot);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_6, 0, 0, 1, 1);

        comboX = new QComboBox(tabPlot);
        comboX->setObjectName(QStringLiteral("comboX"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboX->sizePolicy().hasHeightForWidth());
        comboX->setSizePolicy(sizePolicy1);
        comboX->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        gridLayout->addWidget(comboX, 0, 1, 1, 1);

        label_7 = new QLabel(tabPlot);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_7, 1, 0, 1, 1);

        comboY = new QComboBox(tabPlot);
        comboY->setObjectName(QStringLiteral("comboY"));
        sizePolicy1.setHeightForWidth(comboY->sizePolicy().hasHeightForWidth());
        comboY->setSizePolicy(sizePolicy1);
        comboY->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        gridLayout->addWidget(comboY, 1, 1, 1, 1);

        label_8 = new QLabel(tabPlot);
        label_8->setObjectName(QStringLiteral("label_8"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy2);
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_8, 2, 0, 1, 1);

        pointSizeSpinBox = new QSpinBox(tabPlot);
        pointSizeSpinBox->setObjectName(QStringLiteral("pointSizeSpinBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(pointSizeSpinBox->sizePolicy().hasHeightForWidth());
        pointSizeSpinBox->setSizePolicy(sizePolicy3);
        pointSizeSpinBox->setMinimum(1);
        pointSizeSpinBox->setMaximum(15);
        pointSizeSpinBox->setValue(3);

        gridLayout->addWidget(pointSizeSpinBox, 2, 1, 1, 1);

        pointSizeSlider = new QSlider(tabPlot);
        pointSizeSlider->setObjectName(QStringLiteral("pointSizeSlider"));
        sizePolicy1.setHeightForWidth(pointSizeSlider->sizePolicy().hasHeightForWidth());
        pointSizeSlider->setSizePolicy(sizePolicy1);
        pointSizeSlider->setMinimum(1);
        pointSizeSlider->setMaximum(15);
        pointSizeSlider->setValue(3);
        pointSizeSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(pointSizeSlider, 3, 0, 1, 2);

        label_10 = new QLabel(tabPlot);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_10, 4, 0, 1, 1);

        firstSpinBox = new QSpinBox(tabPlot);
        firstSpinBox->setObjectName(QStringLiteral("firstSpinBox"));

        gridLayout->addWidget(firstSpinBox, 4, 1, 1, 1);

        firstSlider = new QSlider(tabPlot);
        firstSlider->setObjectName(QStringLiteral("firstSlider"));
        sizePolicy1.setHeightForWidth(firstSlider->sizePolicy().hasHeightForWidth());
        firstSlider->setSizePolicy(sizePolicy1);
        firstSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(firstSlider, 5, 0, 1, 2);

        label_9 = new QLabel(tabPlot);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_9, 6, 0, 1, 1);

        lastSpinBox = new QSpinBox(tabPlot);
        lastSpinBox->setObjectName(QStringLiteral("lastSpinBox"));

        gridLayout->addWidget(lastSpinBox, 6, 1, 1, 1);

        lastSlider = new QSlider(tabPlot);
        lastSlider->setObjectName(QStringLiteral("lastSlider"));
        sizePolicy1.setHeightForWidth(lastSlider->sizePolicy().hasHeightForWidth());
        lastSlider->setSizePolicy(sizePolicy1);
        lastSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(lastSlider, 7, 0, 1, 2);

        sampleWeightCheckBox = new QCheckBox(tabPlot);
        sampleWeightCheckBox->setObjectName(QStringLiteral("sampleWeightCheckBox"));

        gridLayout->addWidget(sampleWeightCheckBox, 8, 0, 1, 2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 10, 0, 1, 2);

        mergeReplicationsCheckBox = new QCheckBox(tabPlot);
        mergeReplicationsCheckBox->setObjectName(QStringLiteral("mergeReplicationsCheckBox"));
        mergeReplicationsCheckBox->setChecked(true);

        gridLayout->addWidget(mergeReplicationsCheckBox, 9, 0, 1, 2);


        verticalLayout_5->addLayout(gridLayout);


        horizontalLayout_7->addLayout(verticalLayout_5);

        tabWidget->addTab(tabPlot, QString());
        tabMatrix = new QWidget();
        tabMatrix->setObjectName(QStringLiteral("tabMatrix"));
        horizontalLayout_5 = new QHBoxLayout(tabMatrix);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        matrixTable = new QTableWidget(tabMatrix);
        matrixTable->setObjectName(QStringLiteral("matrixTable"));
        matrixTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        matrixTable->setTabKeyNavigation(false);
        matrixTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        matrixTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

        horizontalLayout_5->addWidget(matrixTable);

        tabWidget->addTab(tabMatrix, QString());
        tabEigen = new QWidget();
        tabEigen->setObjectName(QStringLiteral("tabEigen"));
        horizontalLayout_6 = new QHBoxLayout(tabEigen);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        eigenTable = new QTableWidget(tabEigen);
        eigenTable->setObjectName(QStringLiteral("eigenTable"));
        eigenTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        eigenTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

        horizontalLayout_6->addWidget(eigenTable);

        tabWidget->addTab(tabEigen, QString());
        tabMax = new QWidget();
        tabMax->setObjectName(QStringLiteral("tabMax"));
        horizontalLayout_8 = new QHBoxLayout(tabMax);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        maxTable = new QTableWidget(tabMax);
        maxTable->setObjectName(QStringLiteral("maxTable"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(2);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(maxTable->sizePolicy().hasHeightForWidth());
        maxTable->setSizePolicy(sizePolicy4);
        maxTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        maxTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

        horizontalLayout_8->addWidget(maxTable);

        tabWidget->addTab(tabMax, QString());
        tabWinRate = new QWidget();
        tabWinRate->setObjectName(QStringLiteral("tabWinRate"));
        horizontalLayout_9 = new QHBoxLayout(tabWinRate);
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        winRateTable = new QTableWidget(tabWinRate);
        winRateTable->setObjectName(QStringLiteral("winRateTable"));

        horizontalLayout_9->addWidget(winRateTable);

        tabWidget->addTab(tabWinRate, QString());

        horizontalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 673, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QStringLiteral("menu_Help"));
        menu_Experiment = new QMenu(menuBar);
        menu_Experiment->setObjectName(QStringLiteral("menu_Experiment"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        QWidget::setTabOrder(tabWidget, textEdit);
        QWidget::setTabOrder(textEdit, listWidget);
        QWidget::setTabOrder(listWidget, comboX);
        QWidget::setTabOrder(comboX, comboY);
        QWidget::setTabOrder(comboY, pointSizeSpinBox);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menu_Experiment->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menuFile->addAction(action_Open);
        menuFile->addAction(action_Close);
        menuFile->addAction(action_Quit);
        menu_Help->addAction(action_About_Qt);
        menu_Help->addAction(action_About);
        menu_Experiment->addAction(action_Start);
        menu_Experiment->addAction(action_SoftStop);
        menu_Experiment->addAction(action_HardStop);
        menu_Experiment->addSeparator();
        menu_Experiment->addAction(action_Automatic_GUI_update);
        menu_Experiment->addAction(action_Update_GUI_manually_now);

        retranslateUi(MainWindow);
        QObject::connect(action_Quit, SIGNAL(triggered()), MainWindow, SLOT(close()));
        QObject::connect(comboY, SIGNAL(currentIndexChanged(int)), plotWidget, SLOT(update()));
        QObject::connect(firstSpinBox, SIGNAL(valueChanged(int)), firstSlider, SLOT(setValue(int)));
        QObject::connect(pointSizeSlider, SIGNAL(valueChanged(int)), pointSizeSpinBox, SLOT(setValue(int)));
        QObject::connect(lastSpinBox, SIGNAL(valueChanged(int)), plotWidget, SLOT(update()));
        QObject::connect(pointSizeSpinBox, SIGNAL(valueChanged(int)), plotWidget, SLOT(update()));
        QObject::connect(lastSpinBox, SIGNAL(valueChanged(int)), lastSlider, SLOT(setValue(int)));
        QObject::connect(lastSlider, SIGNAL(valueChanged(int)), lastSpinBox, SLOT(setValue(int)));
        QObject::connect(comboX, SIGNAL(currentIndexChanged(int)), plotWidget, SLOT(update()));
        QObject::connect(firstSlider, SIGNAL(valueChanged(int)), firstSpinBox, SLOT(setValue(int)));
        QObject::connect(pointSizeSpinBox, SIGNAL(valueChanged(int)), pointSizeSlider, SLOT(setValue(int)));
        QObject::connect(firstSpinBox, SIGNAL(valueChanged(int)), plotWidget, SLOT(update()));
        QObject::connect(sampleWeightCheckBox, SIGNAL(stateChanged(int)), plotWidget, SLOT(update()));
        QObject::connect(action_Automatic_GUI_update, SIGNAL(toggled(bool)), action_Update_GUI_manually_now, SLOT(setDisabled(bool)));
        QObject::connect(action_Update_GUI_manually_now, SIGNAL(triggered()), MainWindow, SLOT(updateAll()));
        QObject::connect(action_Automatic_GUI_update, SIGNAL(toggled(bool)), MainWindow, SLOT(updateConnections()));
        QObject::connect(mergeReplicationsCheckBox, SIGNAL(stateChanged(int)), plotWidget, SLOT(update()));

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QString());
        action_Open->setText(QApplication::translate("MainWindow", "&Open...", Q_NULLPTR));
        action_About->setText(QApplication::translate("MainWindow", "&About...", Q_NULLPTR));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", Q_NULLPTR));
        action_Start->setText(QApplication::translate("MainWindow", "&Start", Q_NULLPTR));
        action_SoftStop->setText(QApplication::translate("MainWindow", "S&oft Stop (wait for running games to finish)", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        action_SoftStop->setToolTip(QApplication::translate("MainWindow", "Soft Stop", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        action_HardStop->setText(QApplication::translate("MainWindow", "&Hard Stop (kill everything)", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        action_HardStop->setToolTip(QApplication::translate("MainWindow", "Hard Stop", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        action_Close->setText(QApplication::translate("MainWindow", "&Close", Q_NULLPTR));
        action_About_Qt->setText(QApplication::translate("MainWindow", "About Qt...", Q_NULLPTR));
        action_Automatic_GUI_update->setText(QApplication::translate("MainWindow", "&Automatic GUI update", Q_NULLPTR));
        action_Update_GUI_manually_now->setText(QApplication::translate("MainWindow", "&Update GUI manually now", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabExperiment), QApplication::translate("MainWindow", "&Experiment", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabLog), QApplication::translate("MainWindow", "&Log", Q_NULLPTR));
        label_6->setText(QApplication::translate("MainWindow", "X", Q_NULLPTR));
        label_7->setText(QApplication::translate("MainWindow", "Y", Q_NULLPTR));
        label_8->setText(QApplication::translate("MainWindow", "PSize", Q_NULLPTR));
        label_10->setText(QApplication::translate("MainWindow", "First", Q_NULLPTR));
        label_9->setText(QApplication::translate("MainWindow", "Last", Q_NULLPTR));
        sampleWeightCheckBox->setText(QApplication::translate("MainWindow", "Sample Weight", Q_NULLPTR));
        mergeReplicationsCheckBox->setText(QApplication::translate("MainWindow", "Merge Replications", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabPlot), QApplication::translate("MainWindow", "&Plot", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabMatrix), QApplication::translate("MainWindow", "&Hessian", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabEigen), QApplication::translate("MainWindow", "Ei&genvectors", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabMax), QApplication::translate("MainWindow", "&Max", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tabWinRate), QApplication::translate("MainWindow", "Win Rate", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindow", "&File", Q_NULLPTR));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", Q_NULLPTR));
        menu_Experiment->setTitle(QApplication::translate("MainWindow", "&Experiment", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
