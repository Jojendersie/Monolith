#include "cubeview.h"

#include <QtWidgets/QComboBox>

#include <Qt3D/qglbuilder>
#include <Qt3D/qglcube>
#include <QMouseEvent>
#include <QGLPickNode>
#include <QGLAbstractScene>

#include <algorithm>
#include <functional>

//makro to iterate throught the voxel
#define FOREACH for(int x = 0; x < m_res; x++) for(int y = 0; y < m_res; y++) for(int z = 0; z < m_res; z++)

CubeView::CubeView(QComboBox* _colorBox, int _res, QWidget *parent)
    : QGLView(),
	m_object(nullptr),
	m_colorBox(_colorBox),
	m_selected(nullptr)
{
//	connect(_colorBox, SIGNAL(currentIndexChanged(QString &)),this, SLOT(colorChanged(QString &)));
	//setup options
	setOption(QGLView::ObjectPicking, true);
	setOption(QGLView::ShowPicking, false);
	glEnable(GL_BLEND);
	
	// Set up sizes
	m_res = _res;

	m_offset = (-m_res/5.0);// /2.5 to center it; /2 blocks only have a size of 0.5

	float linOffset = m_offset+ (m_res-1)*0.5;

	m_axisOffset = -QVector3D(linOffset, linOffset, linOffset);

	m_cube.resize(m_res);
	for (int i = 0; i < m_res; ++i) 
	{
		m_cube[i].resize(m_res);
		for (int j = 0; j < m_res; ++j)
			m_cube[i][j].resize(m_res);
	} 
	m_cubeData.resize(m_res);
	for (int i = 0; i < m_res; ++i) 
	{
		m_cubeData[i].resize(m_res);
		for (int j = 0; j < m_res; ++j)
			m_cubeData[i][j].resize(m_res);
	}  

    FOREACH
	{
		//build cube data first
		m_cubeData[x][y][z] = new Cube();

		//build cube
		QGLBuilder builder;
		builder << QGL::Faceted;
		builder << QGLCube(0.44);//0.5
	
		m_cube[x][y][z] = builder.finalizedSceneNode();
	//	m_cube[ix][iy][iz]->setParent(&pickScene);
		m_pickScene.mainNode()->addNode(m_cube[x][y][z]);
				
		//arrange
		m_cube[x][y][z]->setPosition(QVector3D(m_offset+x*0.5, m_offset+y*0.5, m_offset+z*0.5));

		m_cubeData[x][y][z]->setNode(m_cube[x][y][z]);
		m_cubeData[x][y][z]->locX = x;
		m_cubeData[x][y][z]->locY = y;
		m_cubeData[x][y][z]->locZ = z;
	}
	m_pickScene.setPickable(true);
	QList<QGLPickNode *>nodes = m_pickScene.pickNodes();
	//int arafasg = nodes.length();
	foreach (QGLPickNode *node, nodes)
	{
		registerObject(node->id(), node);
	//	connect(node, SIGNAL(clicked()),this, SLOT(objectPicked()));
	}
}

CubeView::~CubeView()
{
	FOREACH 
	{
		delete m_cube[x][y][z];
		delete m_cubeData[x][y][z];
	}
}
 
void CubeView::setTex( unsigned int* _tex )
{
	FOREACH
	{
		unsigned int col = _tex[x + y*m_res + z*m_res*m_res];
		m_cubeData[x][y][z]->setColor(col);
		m_cubeData[x][y][z]->setState(col);
	}
}

void CubeView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().rotate(45.0f, 0.0f, 0.0f, 0.0f);
	
	painter->setStandardEffect(QGL::StandardEffect::FlatColor);
	QArray<QVector3D> line;
	line[0] = m_axisOffset;

	//x
	painter->setColor(QColor(255,0,0));
	line[1] = m_axisOffset + QVector3D(m_res*0.5f, 0.f, 0.f);
	painter->clearAttributes();
	painter->setVertexAttribute(QGL::Position, QGLAttributeValue(line));
    painter->draw(QGL::LineStrip, 2);

	//y
	painter->setColor(QColor(0,255,0));
	line[1] = m_axisOffset + QVector3D(0.f, m_res*0.5f, 0.f);
	painter->clearAttributes();
	painter->setVertexAttribute(QGL::Position, QGLAttributeValue(line));
    painter->draw(QGL::LineStrip, 2);

	//z
	painter->setColor(QColor(0,0,255));
	line[1] = m_axisOffset + QVector3D(0.f, 0.f, m_res*0.5f);
	painter->clearAttributes();
	painter->setVertexAttribute(QGL::Position, QGLAttributeValue(line));
    painter->draw(QGL::LineStrip, 2);

	painter->setStandardEffect(QGL::LitMaterial);

	FOREACH
	{
	//	QGLMaterial material;
	//	material.setColor
		
		painter->setColor(m_cubeData[x][y][z]->m_color);
		 painter->setFaceColor(QGL::AllFaces, m_cubeData[x][y][z]->m_color);
	//	painter->setFaceColor(QGL::AllFaces, m_cubeData[x][y][z]->m_color);
		m_cube[x][y][z]->draw(painter);

		painter->setColor(QColor(255,255,255));
		//geometry is stored in the child nodes
/*		foreach (QGLSceneNode *node, m_cube[x][y][z]->allChildren())
		{
			node->setDrawingMode(QGL::LineStrip);//LineStrip
			node->setDrawingWidth(2);
			//drawing still happens through the root node
			m_cube[x][y][z]->draw(painter);
			//turn back to normal
			node->setDrawingMode(QGL::Triangles);
		}*/
	}
}

Cube* CubeView::getMouseFocus(bool _real)
{
	QGLPickNode* pickNode = dynamic_cast<QGLPickNode*>(objectForPoint(m_mPos));
	if(!pickNode) return nullptr;
	m_object = (QGLSceneNode*)(pickNode->target()->parent());
	//retrieve array position
	int locX = (m_object->x()-m_offset)/0.5;
	int locY = (m_object->y()-m_offset)/0.5;
	int locZ = (m_object->z()-m_offset)/0.5;
	
	//just return the direct pick
	if(_real) return m_cubeData[locX][locY][locZ]; 

	//broadcast to all neighbours
	if(locX+1 < m_res) m_cubeData[locX+1][locY][locZ]->setState(true);
	if(locX-1 >= 0) m_cubeData[locX-1][locY][locZ]->setState(true);
	if(locY+1 < m_res) m_cubeData[locX][locY+1][locZ]->setState(true);
	if(locY-1 >= 0) m_cubeData[locX][locY-1][locZ]->setState(true);
	if(locZ+1 < m_res) m_cubeData[locX][locY][locZ+1]->setState(true);
	if(locZ-1 >= 0) m_cubeData[locX][locY][locZ-1]->setState(true);

	//second try should pick the cube in the focus
	pickNode = dynamic_cast<QGLPickNode*>(objectForPoint(m_mPos));
	if(pickNode) 
	{
		m_object = (QGLSceneNode*)(pickNode->target()->parent());

		//recover the old condition
		if(locX+1 < m_res) m_cubeData[locX+1][locY][locZ]->recoverState();
		if(locX-1 >= 0) m_cubeData[locX-1][locY][locZ]->recoverState();
		if(locY+1 < m_res) m_cubeData[locX][locY+1][locZ]->recoverState();
		if(locY-1 >= 0) m_cubeData[locX][locY-1][locZ]->recoverState();
		if(locZ+1 < m_res) m_cubeData[locX][locY][locZ+1]->recoverState();
		if(locZ-1 >= 0) m_cubeData[locX][locY][locZ-1]->recoverState();

		//retrieve loc for choosen node
		locX = (m_object->x()-m_offset)/0.5;
		locY = (m_object->y()-m_offset)/0.5;
		locZ = (m_object->z()-m_offset)/0.5;

		return m_cubeData[locX][locY][locZ];
	}
	return nullptr;
}


void CubeView::mouseMoveEvent( QMouseEvent* _e )
{
	QGLView::mouseMoveEvent(_e);
	m_mPos = _e->pos();
}

void CubeView::mousePressEvent( QMouseEvent* _e )
{
	QGLView::mousePressEvent(_e);

	m_mPos = _e->pos();
	//save currently selected voxel
	m_selected = getMouseFocus((_e->button() == Qt::LeftButton));
}

void CubeView::mouseReleaseEvent( QMouseEvent* _e )
{
	QGLView::mouseReleaseEvent(_e);
	m_mPos = _e->pos();

	std::function<void(Cube&)> processCube;

	Cube* focus = getMouseFocus(_e->button() == Qt::LeftButton);

	//when their are not two selected do nothing
	if(!focus || !m_selected) return;

	if(_e->button() == Qt::LeftButton)
	{
		processCube = [](Cube& _cube){_cube.setState(false);};
	}
	else if(_e->button() == Qt::RightButton)
	{
		processCube = [&](Cube& _cube){
			colorChanged(m_colorBox->currentText());
			_cube.setColor(m_color);
			_cube.setState(true);
		};
	}

	//get directions for the iteration
	int dx = focus->locX - m_selected->locX > 0 ? 1 : -1;
	int dy = focus->locY - m_selected->locY > 0 ? 1 : -1;
	int dz = focus->locZ - m_selected->locZ > 0 ? 1 : -1;
	//iterate througth the volume
	for(int ix = m_selected->locX; ix != focus->locX + dx; ix += dx)
		for(int iy = m_selected->locY; iy != focus->locY + dy; iy += dy)
			for(int iz = m_selected->locZ; iz != focus->locZ + dz; iz += dz)
				processCube(*m_cubeData[ix][iy][iz]);
/*	if(_e->button() == Qt::LeftButton)
	{
		
		if(Cube* focus = getMouseFocus(true))
			if(!m_selected)
			focus->setState(false);//remove
	}
	else
	{
		if(Cube* focus = getMouseFocus())
		{
			colorChanged(m_colorBox->currentText());
			focus->setColor(m_color);
			focus->setState(true);//add
		}
	}*/
	update();
}


//not in use
void CubeView::objectPicked()
{
	getMouseFocus()->setState(true);
	update();
/*	QGLPickNode* obj = (QGLPickNode*)(sender());
	if(obj) m_object = obj->target();//
//	m_object->setOption(QGLSceneNode::Option::HideNode, true);
	FOREACH
	{
		//the first children of the node seems to be the picking box
		if(m_cube[x][y][z]->children().first() == m_object) 
		{
			m_cubeData[x][y][z].m_color = QColor(255,255,255);
			break;//object of interest found
		}
	}*/
}

void CubeView::colorChanged(QString& _s)
{
	bool ok;
	//add prefix so that toUInt handles s as hexvalue
	QString s = "0x"+_s;
	m_color = s.toUInt(&ok,0);
}

void CubeView::changeCubes(unsigned int _color, unsigned int _newColor, bool _state)
{
	FOREACH
	{
		if(m_cubeData[x][y][z]->m_colorOrg == _color)
		{
			m_cubeData[x][y][z]->setColor(_newColor);
			m_cubeData[x][y][z]->setState(_state);
		}
	}
	//show changes
	update();
}