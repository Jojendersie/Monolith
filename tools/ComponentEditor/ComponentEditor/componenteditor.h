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
	//remove currently selected color
    void on_BtnColorRemove_clicked();

	//swap normal<>border mode
	void on_pushButtonSwap_clicked();
	//new voxel
	void on_pushButtonNew_clicked();
	//new voxel as copy
	void on_pushButtonCopy_clicked();
	//
	void on_pushButtonRemove_clicked();
	//creates a filedialog, loads the chosen file and sets up the editor
	void open();
	//creates an filedialog and saves all voxel into the chosen file, erasing its previous content
	void save();
	//triggered when a voxel is chosen in the combo box
	void voxelChosen( const QString & _text );

	//updates the the currently selected voxel's data from the mask
	void updateVoxelData() { saveModelChanges(); updateAttributes(); };

	//a cell in the attribute table got changed
	void cellChanged ( int row, int column );

	void on_pushButtonAtr_clicked();
private:
	//modes: 0 - border; 1 - normal
	//!caution! does no savety tests
	void saveModelChanges();

	//attributes -> table
	void updateTable();
	//table -> attributes
	void updateAttributes();

	void closeEvent(QCloseEvent *event);

	bool m_mode;

	QPushButton *ui_pushButton;
    QLineEdit *ui_lineEdit;

	Voxel* m_voxels[255];
	Voxel* m_voxel; //currently active voxel
	int m_currentIndex; //index of active voxel

	int m_voxelCount;
	CubeView* m_view;//the active view

	QString m_fileName;
};

#endif // COMPONENTEDITOR_H
