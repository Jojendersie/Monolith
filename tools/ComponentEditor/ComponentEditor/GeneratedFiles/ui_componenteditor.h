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
#include <QtWidgets/QCheckBox>
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
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ComponentEditorClass
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionAbout;
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QComboBox *comboBox;
    QPushButton *BtnColorRemove;
    QGroupBox *generalInfos;
    QLineEdit *editMass;
    QLabel *label_3;
    QLineEdit *editTreshHoldE;
    QLabel *label_4;
    QLineEdit *editReactionE;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *editHydrogen;
    QLabel *label_7;
    QLineEdit *editCarbon;
    QLabel *label_8;
    QLabel *label_9;
    QLineEdit *editMetals;
    QLineEdit *editRare;
    QLabel *label_10;
    QLabel *label_11;
    QLineEdit *editSemicon;
    QLineEdit *editHeisium;
    QCheckBox *checkBoxSolid;
    QLabel *label_12;
    QLabel *label_13;
    QLineEdit *editName;
    QGroupBox *groupBox_2;
    QLineEdit *lineEdit_2;
    QPushButton *pushButtonNew;
    QComboBox *comboBox_2;
    QPushButton *pushButtonRemove;
    QLabel *label_14;
    QLineEdit *editTexres;
    QLabel *label_2;
    QPushButton *pushButtonSwap;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuInfo;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ComponentEditorClass)
    {
        if (ComponentEditorClass->objectName().isEmpty())
            ComponentEditorClass->setObjectName(QStringLiteral("ComponentEditorClass"));
        ComponentEditorClass->resize(238, 514);
        actionOpen = new QAction(ComponentEditorClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionSave = new QAction(ComponentEditorClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionAbout = new QAction(ComponentEditorClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralWidget = new QWidget(ComponentEditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 110, 211, 81));
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
        generalInfos->setGeometry(QRect(10, 190, 211, 271));
        editMass = new QLineEdit(generalInfos);
        editMass->setObjectName(QStringLiteral("editMass"));
        editMass->setGeometry(QRect(60, 50, 131, 20));
        editMass->setClearButtonEnabled(false);
        label_3 = new QLabel(generalInfos);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(8, 50, 46, 13));
        editTreshHoldE = new QLineEdit(generalInfos);
        editTreshHoldE->setObjectName(QStringLiteral("editTreshHoldE"));
        editTreshHoldE->setGeometry(QRect(60, 90, 131, 20));
        label_4 = new QLabel(generalInfos);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 70, 101, 16));
        editReactionE = new QLineEdit(generalInfos);
        editReactionE->setObjectName(QStringLiteral("editReactionE"));
        editReactionE->setGeometry(QRect(60, 130, 131, 20));
        label_5 = new QLabel(generalInfos);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 110, 101, 16));
        label_6 = new QLabel(generalInfos);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(8, 160, 61, 16));
        editHydrogen = new QLineEdit(generalInfos);
        editHydrogen->setObjectName(QStringLiteral("editHydrogen"));
        editHydrogen->setGeometry(QRect(60, 160, 31, 20));
        editHydrogen->setClearButtonEnabled(false);
        label_7 = new QLabel(generalInfos);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(108, 160, 61, 16));
        editCarbon = new QLineEdit(generalInfos);
        editCarbon->setObjectName(QStringLiteral("editCarbon"));
        editCarbon->setGeometry(QRect(160, 160, 31, 20));
        editCarbon->setClearButtonEnabled(false);
        label_8 = new QLabel(generalInfos);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(8, 190, 61, 16));
        label_9 = new QLabel(generalInfos);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(108, 190, 61, 16));
        editMetals = new QLineEdit(generalInfos);
        editMetals->setObjectName(QStringLiteral("editMetals"));
        editMetals->setGeometry(QRect(60, 190, 31, 20));
        editMetals->setClearButtonEnabled(false);
        editRare = new QLineEdit(generalInfos);
        editRare->setObjectName(QStringLiteral("editRare"));
        editRare->setGeometry(QRect(160, 190, 31, 20));
        editRare->setClearButtonEnabled(false);
        label_10 = new QLabel(generalInfos);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(8, 220, 61, 16));
        label_11 = new QLabel(generalInfos);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(108, 220, 61, 16));
        editSemicon = new QLineEdit(generalInfos);
        editSemicon->setObjectName(QStringLiteral("editSemicon"));
        editSemicon->setGeometry(QRect(60, 220, 31, 20));
        editSemicon->setClearButtonEnabled(false);
        editHeisium = new QLineEdit(generalInfos);
        editHeisium->setObjectName(QStringLiteral("editHeisium"));
        editHeisium->setGeometry(QRect(160, 220, 31, 20));
        editHeisium->setClearButtonEnabled(false);
        checkBoxSolid = new QCheckBox(generalInfos);
        checkBoxSolid->setObjectName(QStringLiteral("checkBoxSolid"));
        checkBoxSolid->setGeometry(QRect(60, 250, 16, 17));
        label_12 = new QLabel(generalInfos);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(10, 250, 41, 16));
        label_13 = new QLabel(generalInfos);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(10, 20, 46, 13));
        editName = new QLineEdit(generalInfos);
        editName->setObjectName(QStringLiteral("editName"));
        editName->setGeometry(QRect(62, 20, 131, 20));
        editName->setClearButtonEnabled(false);
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 20, 211, 91));
        lineEdit_2 = new QLineEdit(groupBox_2);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(20, 30, 81, 20));
        pushButtonNew = new QPushButton(groupBox_2);
        pushButtonNew->setObjectName(QStringLiteral("pushButtonNew"));
        pushButtonNew->setGeometry(QRect(170, 30, 31, 21));
        comboBox_2 = new QComboBox(groupBox_2);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));
        comboBox_2->setGeometry(QRect(20, 60, 81, 22));
        pushButtonRemove = new QPushButton(groupBox_2);
        pushButtonRemove->setObjectName(QStringLiteral("pushButtonRemove"));
        pushButtonRemove->setGeometry(QRect(120, 60, 81, 23));
        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(120, 10, 41, 16));
        editTexres = new QLineEdit(groupBox_2);
        editTexres->setObjectName(QStringLiteral("editTexres"));
        editTexres->setGeometry(QRect(120, 30, 31, 20));
        editTexres->setClearButtonEnabled(false);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 10, 46, 13));
        pushButtonSwap = new QPushButton(centralWidget);
        pushButtonSwap->setObjectName(QStringLiteral("pushButtonSwap"));
        pushButtonSwap->setGeometry(QRect(20, 0, 191, 23));
        ComponentEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ComponentEditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 238, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuInfo = new QMenu(menuBar);
        menuInfo->setObjectName(QStringLiteral("menuInfo"));
        ComponentEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ComponentEditorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ComponentEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ComponentEditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ComponentEditorClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuInfo->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuInfo->addAction(actionAbout);

        retranslateUi(ComponentEditorClass);

        QMetaObject::connectSlotsByName(ComponentEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *ComponentEditorClass)
    {
        ComponentEditorClass->setWindowTitle(QApplication::translate("ComponentEditorClass", "ComponentEditor", 0));
        actionOpen->setText(QApplication::translate("ComponentEditorClass", "open", 0));
        actionSave->setText(QApplication::translate("ComponentEditorClass", "save", 0));
        actionAbout->setText(QApplication::translate("ComponentEditorClass", "about", 0));
        groupBox->setTitle(QApplication::translate("ComponentEditorClass", "ColorOptions", 0));
        pushButton->setText(QApplication::translate("ComponentEditorClass", "+", 0));
        BtnColorRemove->setText(QApplication::translate("ComponentEditorClass", "x", 0));
        generalInfos->setTitle(QApplication::translate("ComponentEditorClass", "general Informations", 0));
        label_3->setText(QApplication::translate("ComponentEditorClass", "Mass:", 0));
        editTreshHoldE->setText(QString());
        label_4->setText(QApplication::translate("ComponentEditorClass", "Threshold Energy:", 0));
        editReactionE->setText(QString());
        label_5->setText(QApplication::translate("ComponentEditorClass", "Reaction Energy:", 0));
        label_6->setText(QApplication::translate("ComponentEditorClass", "Hydrogen:", 0));
        label_7->setText(QApplication::translate("ComponentEditorClass", "Carbon:", 0));
        label_8->setText(QApplication::translate("ComponentEditorClass", "Metals:", 0));
        label_9->setText(QApplication::translate("ComponentEditorClass", "Rare:", 0));
        label_10->setText(QApplication::translate("ComponentEditorClass", "Semicon:", 0));
        label_11->setText(QApplication::translate("ComponentEditorClass", "Heisium:", 0));
        editSemicon->setText(QString());
        editHeisium->setText(QString());
        checkBoxSolid->setText(QString());
        label_12->setText(QApplication::translate("ComponentEditorClass", "Solid:", 0));
        label_13->setText(QApplication::translate("ComponentEditorClass", "Name:", 0));
        groupBox_2->setTitle(QApplication::translate("ComponentEditorClass", "Voxels", 0));
        pushButtonNew->setText(QApplication::translate("ComponentEditorClass", "+", 0));
        pushButtonRemove->setText(QApplication::translate("ComponentEditorClass", "remove", 0));
        label_14->setText(QApplication::translate("ComponentEditorClass", "Texres:", 0));
        editTexres->setText(QString());
        label_2->setText(QApplication::translate("ComponentEditorClass", "Name:", 0));
        pushButtonSwap->setText(QApplication::translate("ComponentEditorClass", "bordertexture", 0));
        menuFile->setTitle(QApplication::translate("ComponentEditorClass", "file", 0));
        menuInfo->setTitle(QApplication::translate("ComponentEditorClass", "Info", 0));
    } // retranslateUi

};

namespace Ui {
    class ComponentEditorClass: public Ui_ComponentEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPONENTEDITOR_H
