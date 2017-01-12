/********************************************************************************
** Form generated from reading UI file 'componenteditor.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPONENTEDITOR_H
#define UI_COMPONENTEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ComponentEditorClass
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionAbout;
    QAction *actionClear_colors;
    QAction *actionShow_all;
    QAction *actionToggle_hidden;
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QComboBox *comboBox;
    QPushButton *BtnColorRemove;
    QGroupBox *generalInfos;
    QTableWidget *attributeTable;
    QGroupBox *groupBox_2;
    QLineEdit *lineEdit_2;
    QPushButton *pushButtonNew;
    QComboBox *comboBox_2;
    QPushButton *pushButtonRemove;
    QLabel *label_14;
    QLineEdit *editTexres;
    QLabel *label_2;
    QPushButton *pushButtonCopy;
    QLabel *label_3;
    QPushButton *pushButtonMode;
    QPushButton *pushButtonSwap;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuInfo;
    QMenu *menuEdit;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ComponentEditorClass)
    {
        if (ComponentEditorClass->objectName().isEmpty())
            ComponentEditorClass->setObjectName(QStringLiteral("ComponentEditorClass"));
        ComponentEditorClass->resize(238, 516);
        actionOpen = new QAction(ComponentEditorClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave = new QAction(ComponentEditorClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionAbout = new QAction(ComponentEditorClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionClear_colors = new QAction(ComponentEditorClass);
        actionClear_colors->setObjectName(QStringLiteral("actionClear_colors"));
        actionShow_all = new QAction(ComponentEditorClass);
        actionShow_all->setObjectName(QStringLiteral("actionShow_all"));
        actionToggle_hidden = new QAction(ComponentEditorClass);
        actionToggle_hidden->setObjectName(QStringLiteral("actionToggle_hidden"));
        centralWidget = new QWidget(ComponentEditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setEnabled(false);
        groupBox->setGeometry(QRect(10, 130, 211, 81));
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(20, 20, 141, 20));
        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(170, 20, 31, 23));
        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(20, 50, 141, 22));
        BtnColorRemove = new QPushButton(groupBox);
        BtnColorRemove->setObjectName(QStringLiteral("BtnColorRemove"));
        BtnColorRemove->setGeometry(QRect(170, 50, 31, 23));
        generalInfos = new QGroupBox(centralWidget);
        generalInfos->setObjectName(QStringLiteral("generalInfos"));
        generalInfos->setEnabled(false);
        generalInfos->setGeometry(QRect(10, 210, 221, 281));
        attributeTable = new QTableWidget(generalInfos);
        if (attributeTable->columnCount() < 2)
            attributeTable->setColumnCount(2);
        attributeTable->setObjectName(QStringLiteral("attributeTable"));
        attributeTable->setGeometry(QRect(10, 20, 201, 251));
        attributeTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
        attributeTable->setAlternatingRowColors(true);
        attributeTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        attributeTable->setRowCount(0);
        attributeTable->setColumnCount(2);
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setEnabled(false);
        groupBox_2->setGeometry(QRect(10, 20, 221, 111));
        lineEdit_2 = new QLineEdit(groupBox_2);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(10, 30, 121, 20));
        pushButtonNew = new QPushButton(groupBox_2);
        pushButtonNew->setObjectName(QStringLiteral("pushButtonNew"));
        pushButtonNew->setGeometry(QRect(180, 30, 31, 21));
        comboBox_2 = new QComboBox(groupBox_2);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));
        comboBox_2->setGeometry(QRect(10, 60, 121, 22));
        pushButtonRemove = new QPushButton(groupBox_2);
        pushButtonRemove->setObjectName(QStringLiteral("pushButtonRemove"));
        pushButtonRemove->setGeometry(QRect(180, 60, 31, 23));
        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(130, 10, 41, 16));
        editTexres = new QLineEdit(groupBox_2);
        editTexres->setObjectName(QStringLiteral("editTexres"));
        editTexres->setGeometry(QRect(140, 30, 31, 20));
        editTexres->setClearButtonEnabled(false);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 10, 46, 13));
        pushButtonCopy = new QPushButton(groupBox_2);
        pushButtonCopy->setObjectName(QStringLiteral("pushButtonCopy"));
        pushButtonCopy->setGeometry(QRect(140, 60, 31, 23));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 90, 46, 13));
        pushButtonMode = new QPushButton(groupBox_2);
        pushButtonMode->setObjectName(QStringLiteral("pushButtonMode"));
        pushButtonMode->setGeometry(QRect(50, 85, 121, 20));
        pushButtonSwap = new QPushButton(centralWidget);
        pushButtonSwap->setObjectName(QStringLiteral("pushButtonSwap"));
        pushButtonSwap->setEnabled(false);
        pushButtonSwap->setGeometry(QRect(20, 0, 191, 23));
        ComponentEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ComponentEditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 238, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuInfo = new QMenu(menuBar);
        menuInfo->setObjectName(QStringLiteral("menuInfo"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        ComponentEditorClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(ComponentEditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ComponentEditorClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuInfo->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuInfo->addAction(actionAbout);
        menuEdit->addAction(actionClear_colors);
        menuEdit->addAction(actionShow_all);
        menuEdit->addAction(actionToggle_hidden);

        retranslateUi(ComponentEditorClass);

        QMetaObject::connectSlotsByName(ComponentEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *ComponentEditorClass)
    {
        ComponentEditorClass->setWindowTitle(QApplication::translate("ComponentEditorClass", "ComponentEditor", 0));
        actionOpen->setText(QApplication::translate("ComponentEditorClass", "open", 0));
        actionOpen->setShortcut(QApplication::translate("ComponentEditorClass", "Ctrl+O", 0));
        actionSave->setText(QApplication::translate("ComponentEditorClass", "save", 0));
        actionSave->setShortcut(QApplication::translate("ComponentEditorClass", "Ctrl+S", 0));
        actionAbout->setText(QApplication::translate("ComponentEditorClass", "about", 0));
        actionClear_colors->setText(QApplication::translate("ComponentEditorClass", "clear colors", 0));
        actionShow_all->setText(QApplication::translate("ComponentEditorClass", "show all", 0));
        actionShow_all->setIconText(QApplication::translate("ComponentEditorClass", "show all", 0));
#ifndef QT_NO_TOOLTIP
        actionShow_all->setToolTip(QApplication::translate("ComponentEditorClass", "show all hidden voxels and reset", 0));
#endif // QT_NO_TOOLTIP
        actionToggle_hidden->setText(QApplication::translate("ComponentEditorClass", "toggle hidden", 0));
#ifndef QT_NO_TOOLTIP
        actionToggle_hidden->setToolTip(QApplication::translate("ComponentEditorClass", "show / hide voxels without reseting", 0));
#endif // QT_NO_TOOLTIP
        actionToggle_hidden->setShortcut(QApplication::translate("ComponentEditorClass", "Ctrl+T", 0));
        groupBox->setTitle(QApplication::translate("ComponentEditorClass", "ColorOptions", 0));
#ifndef QT_NO_TOOLTIP
        lineEdit->setToolTip(QApplication::translate("ComponentEditorClass", "color code in hex or decimal without prefix", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pushButton->setToolTip(QApplication::translate("ComponentEditorClass", "add color to this palette", 0));
#endif // QT_NO_TOOLTIP
        pushButton->setText(QApplication::translate("ComponentEditorClass", "+", 0));
#ifndef QT_NO_TOOLTIP
        BtnColorRemove->setToolTip(QApplication::translate("ComponentEditorClass", "remove selected color", 0));
#endif // QT_NO_TOOLTIP
        BtnColorRemove->setText(QApplication::translate("ComponentEditorClass", "x", 0));
        generalInfos->setTitle(QApplication::translate("ComponentEditorClass", "attributes", 0));
        groupBox_2->setTitle(QApplication::translate("ComponentEditorClass", "Voxels", 0));
#ifndef QT_NO_TOOLTIP
        pushButtonNew->setToolTip(QApplication::translate("ComponentEditorClass", "add new voxel", 0));
#endif // QT_NO_TOOLTIP
        pushButtonNew->setText(QApplication::translate("ComponentEditorClass", "+", 0));
#ifndef QT_NO_TOOLTIP
        pushButtonRemove->setToolTip(QApplication::translate("ComponentEditorClass", "delete voxel", 0));
#endif // QT_NO_TOOLTIP
        pushButtonRemove->setText(QApplication::translate("ComponentEditorClass", "x", 0));
        label_14->setText(QApplication::translate("ComponentEditorClass", "Texres:", 0));
        editTexres->setText(QApplication::translate("ComponentEditorClass", "16", 0));
        label_2->setText(QApplication::translate("ComponentEditorClass", "Name:", 0));
#ifndef QT_NO_TOOLTIP
        pushButtonCopy->setToolTip(QApplication::translate("ComponentEditorClass", "copy the selected voxel", 0));
#endif // QT_NO_TOOLTIP
        pushButtonCopy->setText(QApplication::translate("ComponentEditorClass", "c", 0));
        label_3->setText(QApplication::translate("ComponentEditorClass", "Mode:", 0));
#ifndef QT_NO_TOOLTIP
        pushButtonMode->setToolTip(QApplication::translate("ComponentEditorClass", "mode used to place voxels", 0));
#endif // QT_NO_TOOLTIP
        pushButtonMode->setText(QApplication::translate("ComponentEditorClass", "set", 0));
#ifndef QT_NO_TOOLTIP
        pushButtonSwap->setToolTip(QApplication::translate("ComponentEditorClass", "swap", 0));
#endif // QT_NO_TOOLTIP
        pushButtonSwap->setText(QApplication::translate("ComponentEditorClass", "bordertexture", 0));
        menuFile->setTitle(QApplication::translate("ComponentEditorClass", "file", 0));
        menuInfo->setTitle(QApplication::translate("ComponentEditorClass", "Info", 0));
        menuEdit->setTitle(QApplication::translate("ComponentEditorClass", "edit", 0));
    } // retranslateUi

};

namespace Ui {
    class ComponentEditorClass: public Ui_ComponentEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPONENTEDITOR_H
