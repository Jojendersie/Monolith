#include "componenteditor.h"
#include "jofilelib.hpp"
#include <QFileDialog>

using namespace std;

ComponentEditor::ComponentEditor(QWidget *parent)
	: QMainWindow(parent), 
	m_view(nullptr),
	m_voxel(nullptr),
	m_voxelCount(0)
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
}

ComponentEditor::~ComponentEditor()
{

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

//add voxel
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

//just copy and past from https://github.com/Jojendersie/Monolith/blob/master/src/voxel/voxel.cpp
void ComponentEditor::open()
{
	//clean up the envoiment
	if(m_voxelCount)
	{
		int i = -1;
		while(i++ < m_voxelCount)
			delete m_voxels[i];
	}
	m_voxelCount = 0;
	m_voxel = nullptr;
	ui.comboBox_2->clear();
	if(m_view) delete m_view;
	m_view = nullptr;
	std::string m_fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("Files (*.json)")).toStdString();
	Jo::Files::MetaFileWrapper infoFile( Jo::Files::HDDFile( m_fileName ), Jo::Files::Format::JSON );

	m_voxelCount = (int)infoFile.RootNode[string("PerVoxelInfo")].Size();

	// The file contains an array with voxel information
	for( int i = 0; i < m_voxelCount; ++i )
	{
		m_voxels[i] = new Voxel;
		auto& voxelNode = infoFile.RootNode[string("PerVoxelInfo")][i];
		Voxel& voxelInfo = *m_voxels[i];
				// TODO: string pooling to avoid construction and destruction of std strings
		voxelInfo.name = voxelNode[string("Name")];
		voxelInfo.isSolid = voxelNode[string("Solid")].Get(true);
		voxelInfo.mass = voxelNode[string("Mass")].Get(1.0f);
		voxelInfo.thresholdEnergy = voxelNode[string("Threshold Energy")].Get(100000.0f);
		voxelInfo.reactionEnergy = voxelNode[string("Reaction Energy")].Get(0.0f);
		voxelInfo.hydrogen = voxelNode[string("Hydrogen")].Get(0);
		voxelInfo.carbon = voxelNode[string("Carbon")].Get(0);
		voxelInfo.metals = voxelNode[string("Metals")].Get(0);
		voxelInfo.rareEarthElements = voxelNode[string("Rare Earth Elements")].Get(0);
		voxelInfo.semiconductors = voxelNode[string("Semiconductors")].Get(0);
		voxelInfo.heisenbergium = voxelNode[string("Heisenbergium")].Get(0);
		// Get volumetric size
		int s = voxelNode[string("Texture Resolution")].Get(0);
		voxelInfo.textureResolution = s;
		Jo::Files::MetaFileWrapper::Node* borderTexNode = nullptr;
		voxelNode.HasChild( string("Border Texture"), &borderTexNode );
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
		QString qname = QString::fromStdString(m_voxels[i]->name);
		ui.comboBox_2->addItem(qname);
	}//end for m_voxelCount
}

void ComponentEditor::save()
{
	//make sure to get the latest changes saved
	voxelChosen(QString("UNDEFINED"));

	Jo::Files::MetaFileWrapper infoFile;
	
	infoFile.RootNode.Add(string("PerVoxelInfo"), Jo::Files::MetaFileWrapper::ElementType::NODE, m_voxelCount);

	for( int i = 0; i < m_voxelCount; i++ )
	{
		auto& voxelNode = infoFile.RootNode[string("PerVoxelInfo")][i];
		Voxel& voxelInfo = *m_voxels[i];

		voxelNode[string("Name")] = voxelInfo.name;
		voxelNode[string("Solid")] = voxelInfo.isSolid;
		voxelNode[string("Mass")] = voxelInfo.mass;
		voxelNode[string("Threshold Energy")] = voxelInfo.thresholdEnergy;
		voxelNode[string("Reaction Energy")] = voxelInfo.reactionEnergy;
		voxelNode[string("Hydrogen")] = voxelInfo.hydrogen;
		voxelNode[string("Carbon")] = voxelInfo.carbon;
		voxelNode[string("Metals")]  = voxelInfo.metals;
		voxelNode[string("Rare Earth Elements")] = voxelInfo.rareEarthElements;
		voxelNode[string("Semiconductors")] = voxelInfo.semiconductors;
		voxelNode[string("Heisenbergium")] = voxelInfo.heisenbergium;

		voxelNode[string("Texture Resolution")] = m_voxels[i]->textureResolution;
		int s = voxelInfo.textureResolution;
		s = s * s * s;
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
		//bordertexture, only in case of existens
		if(voxelInfo.borderTexture)
		{
			auto& borderTexNode = voxelNode.Add(string("Border Texture"), Jo::Files::MetaFileWrapper::ElementType::INT32, s);
			for(int c = 0; c < s; c++)
				for(int v = 0; v < voxelInfo.colCount; v++)
					if(voxelInfo.colors[v] == voxelInfo.texture[c])//look up in colortable to only write the index in the file 
					{
						borderTexNode[c] = v;
						break;
					}
		}
			 
	}

	std::string fileName = QFileDialog::getSaveFileName(this, tr("save File"),"",tr("Files (*.*)")).toStdString();
	Jo::Files::HDDFile hddFile(fileName, 1);
	infoFile.Write( hddFile, Jo::Files::Format::JSON );
}

void ComponentEditor::voxelChosen(const QString & _text)
{
	//save changes to current voxel made in the editor/ mask
	if(m_voxel) 
	{
		m_voxel->name = ui.editName->text().toStdString();
		m_voxel->isSolid = ui.checkBoxSolid->isChecked();
		m_voxel->mass = ui.editMass->text().toFloat();
		m_voxel->thresholdEnergy = ui.editTreshHoldE->text().toFloat();
		m_voxel->reactionEnergy = ui.editReactionE->text().toFloat();
		m_voxel->hydrogen = ui.editHydrogen->text().toInt();
		m_voxel->carbon = ui.editCarbon->text().toInt();
		m_voxel->metals = ui.editMetals->text().toInt();
		m_voxel->rareEarthElements = ui.editRare->text().toInt();
		m_voxel->semiconductors = ui.editSemicon->text().toInt();
		m_voxel->heisenbergium = ui.editHeisium->text().toInt();

		//the existens of a selected voxel implies the exsistens of the view
		saveModelChanges();
	}
	if(m_view) delete m_view;
	ui.comboBox->clear();
	//perform linear search til name matches
	for(int i = 0; i < m_voxelCount; i++)
		if(m_voxels[i]->name == _text.toStdString())
		{
			m_voxel = m_voxels[i];
			//update edits with the voxelinformation
			ui.editName->setText(_text);
			ui.editMass->setText(QString::number(m_voxels[i]->mass));
			ui.editTreshHoldE->setText(QString::number(m_voxels[i]->thresholdEnergy));
			ui.editReactionE->setText(QString::number(m_voxels[i]->reactionEnergy));
			ui.editHydrogen->setText(QString::number(m_voxels[i]->hydrogen));
			ui.editCarbon->setText(QString::number(m_voxels[i]->carbon));
			ui.editMetals->setText(QString::number(m_voxels[i]->metals));
			ui.editRare->setText(QString::number(m_voxels[i]->rareEarthElements));
			ui.editSemicon->setText(QString::number(m_voxels[i]->semiconductors));
			ui.editHeisium->setText(QString::number(m_voxels[i]->heisenbergium));
			ui.checkBoxSolid->setChecked(m_voxels[i]->isSolid);
			
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

void ComponentEditor::saveModelChanges()
{
	unsigned int* tex = m_mode ? m_voxel->texture : m_voxel->borderTexture;
	int res = m_voxel->textureResolution;
	for(int ix = 0; ix < res; ix++) 
		for(int iy = 0; iy < res; iy++) 
			for(int iz = 0; iz < res; iz++)
				tex[ix+iy*res+iz*res*res] = m_view->getState(ix,iy,iz) ? m_view->getCol(ix,iy,iz) : 0;
}