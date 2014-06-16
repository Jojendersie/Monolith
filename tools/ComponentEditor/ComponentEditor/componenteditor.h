#ifndef COMPONENTEDITOR_H
#define COMPONENTEDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_componenteditor.h"
#include "cubeview.h"

class QLineEdit;
class QPushButton;

class ComponentEditor : public QMainWindow
{
	Q_OBJECT

public:
	ComponentEditor(QWidget *parent = 0);
	~ComponentEditor();

	Ui::ComponentEditorClass ui;

	QComboBox* ui_comboBox;

private slots:
	//new color
    void on_pushButton_clicked();
	//swap normal<>border mode
	void on_pushButtonSwap_clicked();
	//new voxel
	void on_pushButtonNew_clicked();
	//creates a filedialog, loads the chosen file and sets up the editor
	void open();
	//creates an filedialog and saves all voxel into the chosen file, erasing its previous content
	void save();
	//triggered when a voxel is chosen in the combo box
	void voxelChosen( const QString & _text );

private:
	//modes: 0 - border; 1 - normal
	//!caution! does no savety tests
	void saveModelChanges();

	bool m_mode;

	QPushButton *ui_pushButton;
    QLineEdit *ui_lineEdit;

	Voxel* m_voxels[255];
	Voxel* m_voxel; //currently active voxel
	int m_voxelCount;
	CubeView* m_view;//the active view

	QString m_fileName;
};

#endif // COMPONENTEDITOR_H
