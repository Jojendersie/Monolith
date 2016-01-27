#include "componenteditor.h"
#include "jofilelib.hpp"
#include <QFileDialog>

using namespace std;

ComponentEditor::ComponentEditor(QWidget *parent)
	: QMainWindow(parent), 
	m_view(nullptr),
	m_voxel(nullptr),
	m_voxelCount(0),
	m_currentIndex(-1)
{
	ui.setupUi(this);
	ui_pushButton = findChild<QPushButton*>("pushButton");
    ui_lineEdit = findChild<QLineEdit*>("lineEdit");
	ui_comboBox = findChild<QComboBox*>("comboBox");

	//connect menu items
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(open()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(save()));
	//connect comboBox
	connect(ui.comboBox_2, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(voxelChosen(const QString&)));
	//attr table
	connect(ui.attributeTable, SIGNAL(cellChanged ( int, int )), this, SLOT(cellChanged ( int, int )));
}

ComponentEditor::~ComponentEditor()
{
	for(int i = 0; i < m_voxelCount; ++i)
		delete m_voxels[i];
}

void ComponentEditor::closeEvent(QCloseEvent *event)
{
	//qts parent modell does not work for QGLView
	if(m_view) m_view->close();
	QMainWindow::closeEvent(event);
}

// add color
void ComponentEditor::on_pushButton_clicked()
{
	QString s = ui_lineEdit->text();
	bool ok;
	unsigned int color = s.toUInt(&ok,16);
	m_voxel->colors.push_back(color);
	//after the first color gets added create one block of it, so that blocks can be placed in the view
	//some voxels need no zero(e. g. computer)
	if(m_voxel->colors.size() <= int(ui_comboBox->findText("0") != -1) + 1)
	{
		m_voxel->texture[0] = color;
		m_view->setTex(m_voxel->texture);
		m_view->update();
	}
	//add to combobox
	QPixmap pixmap(8,8);
	pixmap.fill(Cube::yCbCrToRGB(color));
	ui_comboBox->addItem(QIcon(pixmap),s);
	//set added color as active
	ui_comboBox->setCurrentIndex(ui_comboBox->count() - 1);
	ui_lineEdit->setText(QString(""));
}

//remove color
void ComponentEditor::on_BtnColorRemove_clicked()
{
	QString s = ui_lineEdit->text();
	bool ok;
	unsigned int color = ui_comboBox->currentText().toUInt(&ok,16);
	m_view->changeCubes(color);
	m_voxel->colors.erase(m_voxel->colors.begin() + ui_comboBox->currentIndex());
	ui_comboBox->removeItem(ui_comboBox->currentIndex());
}

//swap bordertex
void ComponentEditor::on_pushButtonSwap_clicked()
{
	saveModelChanges();
	m_mode = !m_mode;
	if(!m_voxel->borderTexture) 
	{
		int s = m_voxel->textureResolution * m_voxel->textureResolution * m_voxel->textureResolution;
		m_voxel->borderTexture = new unsigned int[s];
		//new bordertexture starts as copy of the normal texture
		memcpy(m_voxel->borderTexture, m_voxel->texture, sizeof(unsigned int)*s);
	}
	ui.pushButtonSwap->setText(m_mode ? "swap to border" : "swap to normal");
	m_view->setTex(m_mode ? m_voxel->texture : m_voxel->borderTexture);
	m_view->update();
}

void ComponentEditor::on_pushButtonNew_clicked()
{
	if(ui.lineEdit_2->text() == "") return;

	if(ui.editTexres->text() == "") ui.editTexres->setText("16");

	m_voxels[m_voxelCount] = new Voxel(*m_voxels[0]);
	int res  = ui.editTexres->text().toInt();
	m_voxels[m_voxelCount]->textureResolution = res;
	m_voxels[m_voxelCount]->texture = new unsigned int[res*res*res];
	ZeroMemory(m_voxels[m_voxelCount]->texture, sizeof(unsigned int)*res*res*res);
	QString qname = ui.lineEdit_2->text();
	m_voxels[m_voxelCount]->name = qname.toStdString();
	ui.comboBox_2->addItem(qname);
	m_voxelCount++;

	//clean edits to show that the input has been accepted
	ui.editTexres->setText("");
	ui.lineEdit_2->setText("");
}

void ComponentEditor::on_pushButtonCopy_clicked()
{
	if(!m_voxel) return;
	if(ui.lineEdit_2->text() == "") return;

	m_voxels[m_voxelCount] = new Voxel(*m_voxel);

	//the copy needs its one texture
	int res = m_voxel->textureResolution;
	res = res*res*res;
	QString qname = ui.lineEdit_2->text();
	m_voxels[m_voxelCount]->name = qname.toStdString();
	m_voxels[m_voxelCount]->texture = new unsigned int[res];
	memcpy(m_voxels[m_voxelCount]->texture, m_voxel->texture, res * sizeof(unsigned int));
	if(m_voxel->borderTexture)
	{
		m_voxels[m_voxelCount]->borderTexture = new unsigned int[res];
		memcpy(m_voxels[m_voxelCount]->borderTexture, m_voxel->borderTexture, res * sizeof(unsigned int));
	}
	ui.comboBox_2->addItem(qname);

	m_voxelCount++;

	ui.lineEdit_2->setText("");
}

void ComponentEditor::on_pushButtonRemove_clicked()
{
	//delete and fill place with the last voxel
	delete m_voxels[m_currentIndex];
	m_voxelCount--;
	if(m_currentIndex != m_voxelCount)
		m_voxels[m_currentIndex] = m_voxels[m_voxelCount];
	//reset m_voxel to prevent the saving
	m_voxel = nullptr;
	//remove from selection
	ui.comboBox_2->removeItem(ui.comboBox_2->currentIndex());
}

//just copy and past from https://github.com/Jojendersie/Monolith/blob/master/src/voxel/voxel.cpp
void ComponentEditor::open()
{
	//clean up the envoiment
	if(m_voxelCount)
	{
		int i = -1;
		while(i++ < m_voxelCount - 1)
			delete m_voxels[i];
	}
	m_voxelCount = 0;
	m_voxel = nullptr;
	ui.comboBox_2->clear();
	if(m_view) delete m_view;
	m_view = nullptr;

	std::string m_fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.json)")).toStdString();
	if(!m_fileName.size()) return;

	Jo::Files::MetaFileWrapper infoFile( Jo::Files::HDDFile( m_fileName ), Jo::Files::Format::JSON );

	m_voxelCount = (int)infoFile.RootNode[string("PerVoxelInfo")].Size();

	// The file contains an array with voxel information
	for( int i = 0; i < m_voxelCount; ++i )
	{
		m_voxels[i] = new Voxel;
		auto& voxelNode = infoFile.RootNode[string("PerVoxelInfo")][i];
		Voxel& voxelInfo = *m_voxels[i];
		voxelInfo.name = voxelNode[string("Name")];
		// Get volumetric size
		int s = voxelNode[string("Texture Resolution")].Get(0);
		voxelInfo.textureResolution = s;
		Jo::Files::MetaFileWrapper::Node* borderTexNode = nullptr;
		
		bool hasBorderTex = voxelNode.HasChild( string("Border Texture"), &borderTexNode );

		s = s * s * s;
		// Copy textures while interpreting the lookup table
		voxelInfo.texture = new unsigned int[s];
		if( borderTexNode )
		{
			voxelInfo.borderTexture = new unsigned int[s];
		}
		else
			voxelInfo.borderTexture = nullptr;
		auto& colorNode = voxelNode[string("Colors")];
		voxelInfo.colCount = colorNode.Size();
		voxelInfo.colors.resize(voxelInfo.colCount);
	//	voxelInfo.colors = new unsigned int[voxelInfo.colCount];
		for(int c = 0; c < voxelInfo.colCount; c++)
			voxelInfo.colors[c] = colorNode[c];
		auto& texNode = voxelNode[string("Texture")];
		for( int v = 0; v < s; ++v )
		{
			// Read the textures pixel for pixel and do material lookups in
			// the shared color table.
			int index = (int)texNode[v];
			voxelInfo.texture[v] = colorNode[index];
			// Read border texture only if defined
			if( borderTexNode )
			{
				index = (int)(*borderTexNode)[v];
				voxelInfo.borderTexture[v] = colorNode[index];
			}
		}

		//all other attributes
		for(int index = 0; index < voxelNode.Size(); ++index)
		{
			string name = voxelNode[index].GetName();
			//these attributes are handled different
			if(name != "Colors" && name != "Texture Resolution" && name != "Border Texture" && name != "Texture")
			{
				if(voxelNode[index].IsString())
					voxelInfo.m_attributes.emplace_back(voxelNode[index].GetName(), string(voxelNode[index]));
				else if(voxelNode[index].IsFloat() || voxelNode[index].IsInt())
					voxelInfo.m_attributes.emplace_back(voxelNode[index].GetName(), voxelNode[index].Get(0.f));
				else
					voxelInfo.m_attributes.emplace_back(voxelNode[index].GetName(), voxelNode[index].Get(true));
			}
		}
	}//end for m_voxelCount

	//after the file is fully load add all voxels to the dropdown menu
	for( int i = 0; i < m_voxelCount; ++i )
	{
		QString qname = QString::fromStdString(m_voxels[i]->name);
		ui.comboBox_2->addItem(qname);
	}
}

void ComponentEditor::save()
{
	//show dialog now so that if it is canceled nothing happens
	std::string fileName = QFileDialog::getSaveFileName(this, tr("save File"),"",tr("Files (*.*)")).toStdString();
	if(!fileName.size()) return;

	//make sure to get the latest changes saved
	voxelChosen(QString("UNDEFINED"));

	Jo::Files::MetaFileWrapper infoFile;
	
	infoFile.RootNode.Add(string("PerVoxelInfo"), Jo::Files::MetaFileWrapper::ElementType::NODE, m_voxelCount);

	for( int i = 0; i < m_voxelCount; i++ )
	{
		auto& voxelNode = infoFile.RootNode[string("PerVoxelInfo")][i];
		Voxel& voxelInfo = *m_voxels[i];

		//attributes
		for(auto& atr : voxelInfo.m_attributes)
		{
			switch (atr.type)
			{
			case Float:
				voxelNode[atr.name] = atr.value;
				break;
			case String:
				voxelNode[atr.name] = atr.strValue;
				break;
			case Bool:
				voxelNode[atr.name] = atr.value != 0.f;
				break;
			}
		}

		voxelNode[string("Texture Resolution")] = m_voxels[i]->textureResolution;
		int s = voxelInfo.textureResolution;
		s = s * s * s;

		//make shure to update it
		voxelInfo.colCount = voxelInfo.colors.size();
		auto& colorNode = voxelNode.Add(string("Colors"), Jo::Files::MetaFileWrapper::ElementType::UINT32, voxelInfo.colCount );
		for(int c = 0; c < voxelInfo.colCount; c++)
				colorNode[c] = voxelInfo.colors[c];

		//texture
		auto& texNode = voxelNode.Add(string("Texture"), Jo::Files::MetaFileWrapper::ElementType::INT32, s);
		for(int c = 0; c < s; c++)
			for(int v = 0; v < voxelInfo.colCount; v++)
				if(voxelInfo.colors[v] == voxelInfo.texture[c])//look up in colortable to only write the index in the file 
				{
					texNode[c] = v;
					break;
				}

		//bordertexture, only in case of existence
		if(voxelInfo.borderTexture)
		{
			auto& borderTexNode = voxelNode.Add(string("Border Texture"), Jo::Files::MetaFileWrapper::ElementType::INT32, s);
			for(int c = 0; c < s; c++)
				for(int v = 0; v < voxelInfo.colCount; v++)
					if(voxelInfo.colors[v] == voxelInfo.borderTexture[c])//look up in colortable to only write the index in the file 
					{
						borderTexNode[c] = v;
						break;
					}
		}
			 
	}
	
	//on win10 the filename given by the dialog seems to cause trouble
//	for(int i = 0; i < fileName.length(); ++i)
//		if(fileName[i] == '/') fileName[i] = '\\';
	//absolute paths cause a crash
/*	for(int i = fileName.size() - 1; i >= 0; --i)
		if(fileName[i] == '/')
		{
			fileName.erase(fileName.begin(), fileName.begin() + i + 1);
			break;
		}*/
	Jo::Files::HDDFile hddFile(fileName, Jo::Files::HDDFile::OVERWRITE);
	infoFile.Write( hddFile, Jo::Files::Format::JSON );
}

void ComponentEditor::voxelChosen(const QString & _text)
{
	//save changes to current voxel made in the editor/ mask
	if(m_voxel) 
	{
		updateVoxelData();
	}
	if(m_view)
	{
		delete m_view;
		m_view = nullptr;
	}
	ui.comboBox->clear();

	string str = _text.toStdString();
	//perform linear search til name matches
	for(int i = 0; i < m_voxelCount; i++)
		if(m_voxels[i]->name == str)
		{
			m_currentIndex = i;
			m_voxel = m_voxels[i];
			//update table with the voxelinformation
			updateTable();
/*			ui.editName->setText(_text);
			ui.editMass->setText(QString::number(m_voxels[i]->mass));
			ui.editTreshHoldE->setText(QString::number(m_voxels[i]->thresholdEnergy));
			ui.editReactionE->setText(QString::number(m_voxels[i]->reactionEnergy));
			ui.editHydrogen->setText(QString::number(m_voxels[i]->hydrogen));
			ui.editCarbon->setText(QString::number(m_voxels[i]->carbon));
			ui.editMetals->setText(QString::number(m_voxels[i]->metals));
			ui.editRare->setText(QString::number(m_voxels[i]->rareEarthElements));
			ui.editSemicon->setText(QString::number(m_voxels[i]->semiconductors));
			ui.editHeisium->setText(QString::number(m_voxels[i]->heisenbergium));
			ui.checkBoxSolid->setChecked(m_voxels[i]->isSolid);*/
			
			//add colors to the comboBox
			for(int v = 0; v < m_voxels[i]->colors.size(); v++)
			{
				QPixmap pixmap(8,8);
				pixmap.fill(Cube::yCbCrToRGB(m_voxels[i]->colors[v]));
				ui.comboBox->addItem(QIcon(pixmap), QString::number(m_voxels[i]->colors[v], 16));
			}
			
			//number of iterations for cube fo len textureResolution
			/*int it = m_voxels[i]->textureResolution*m_voxels[i]->textureResolution*m_voxels[i]->textureResolution;//number of iterations for cube fo len textureResolution
			for(int v = 0; v < it; v++)
			{
				bool alreadyIn = false;
				QString s = QString::number(m_voxels[i]->texture[v], 16);
				//zero is not a color
				if(s == "0") continue;
				//check whether it exists already
				for(int c = 0; c < ui.comboBox->count(); c++)
					if(ui.comboBox->itemText(c) == s)
					{
						alreadyIn = true;
						break;
					}
				if(!alreadyIn)
				{
					QPixmap pixmap(8,8);
					bool ok;
					pixmap.fill(Cube::yCbCrToRGB(s.toUInt(&ok,16)));
					ui.comboBox->addItem(QIcon(pixmap), s);
				}
			}*/
			m_mode = true;
			ui.pushButtonSwap->setText(m_voxel->borderTexture ? "swap to border" : "create border");
		//	ui.pushButtonSwap->setDisabled(!m_voxel->borderTexture);
			//create a new view for the selected voxel
			m_view = new CubeView(ui_comboBox, m_voxels[i]->textureResolution);
			m_view->setTex(m_voxels[i]->texture);
			m_view->setHeight(512);
			m_view->setWidth(512);
			m_view->setPosition(pos().x()+width()+24,pos().y());
			m_view->show();
			return;
		}
}

// *************************************************** //

void ComponentEditor::on_pushButtonAtr_clicked()
{
	//parent takes ownership
	if(m_voxel)
		(new AttributeTable(this, m_voxel->m_attributes))->show();
}

// *************************************************** //

void ComponentEditor::cellChanged ( int row, int column )
{
	int rowCount = ui.attributeTable->rowCount();
	//last row got accessed
	if(row == rowCount - 1)
	{
		ui.attributeTable->setRowCount(rowCount + 1);
//		ui.attributeTable->setItem(rowCount - 1, 1, new QTableWidgetItem(QString("0.0")));
	}
}

// *************************************************** //

void ComponentEditor::saveModelChanges()
{
	unsigned int* tex = m_mode ? m_voxel->texture : m_voxel->borderTexture;
	int res = m_voxel->textureResolution;
	for(int ix = 0; ix < res; ix++) 
		for(int iy = 0; iy < res; iy++) 
			for(int iz = 0; iz < res; iz++)
				tex[ix+iy*res+iz*res*res] = m_view->getState(ix,iy,iz) ? m_view->getCol(ix,iy,iz) : 0;
}

// *************************************************** //

void ComponentEditor::updateTable()
{
	vector < Attribute >& attributes = m_voxel->m_attributes;
	//add one to allow adding of other attributes
	//but make shure to discard the previous lines first
	ui.attributeTable->setRowCount(attributes.size());
	ui.attributeTable->setRowCount(attributes.size() + 1);
	ui.attributeTable->setColumnCount(2);

//	ui.atrTable->setColumnWidth(0, 200);

	QStringList header;
	header << "name" << "value";
	ui.attributeTable->setHorizontalHeaderLabels(header);
    ui.attributeTable->verticalHeader()->setVisible(false);

	for(int i = 0; i < attributes.size(); ++i)
	{
		QTableWidgetItem* tableItem = new QTableWidgetItem(QString::fromStdString(attributes[i].name));
		
		//the first 5 attributes are mandetory
		if(i <= 4) tableItem->setFlags(tableItem->flags() ^ Qt::ItemIsEditable);

		ui.attributeTable->setItem(i, 0, tableItem);
		if(attributes[i].type == AttributeType::String) tableItem = new QTableWidgetItem(QString::fromStdString(attributes[i].strValue));
		else if(attributes[i].type == AttributeType::Float) tableItem = new QTableWidgetItem(QString::number(attributes[i].value));
		else if(attributes[i].type == AttributeType::Bool) tableItem = new QTableWidgetItem(attributes[i].value == 0.f ? QString("false") : QString("true"));
		tableItem->setFlags(tableItem->flags() | Qt::ItemIsEditable);
		ui.attributeTable->setItem(i, 1, tableItem);
	}
}

// *************************************************** //

void ComponentEditor::updateAttributes()
{
	vector < Attribute >& attributes = m_voxel->m_attributes;

	//last row should be empty
	int rows = ui.attributeTable->rowCount()-1;
	attributes.clear();
	attributes.reserve(rows);
	for(int i = 0; i < rows; ++i)
	{
		
		string name = ui.attributeTable->item(i,0)->text().toStdString();
		QString value = ui.attributeTable->item(i,1)->text();

		//attribute is unusable
		if(name == "" || value == "") continue;

		if(value[0] >= '0' && value[0] <= '9') attributes.emplace_back(name,value.toFloat());
		else if(value == "true") attributes.emplace_back(name,true);
		else if(value == "false") attributes.emplace_back(name,false);
		else attributes.emplace_back(name,value.toStdString());
	}

}