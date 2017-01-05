#include "cubeview.h"

#include <QtWidgets/QComboBox>

#include <Qt3D/qglbuilder>
#include <Qt3D/qglcube>
#include <QMouseEvent>
#include <QGLPickNode>
#include <QGLAbstractScene>

#include <algorithm>
#include <functional>

const size_t MAXUNDOSTEPS = 10;

//makro to iterate throught the voxel
#define FOREACH for(int x = 0; x < m_res; x++) for(int y = 0; y < m_res; y++) for(int z = 0; z < m_res; z++)

CubeView::CubeView(QComboBox* _colorBox, int _res, QWidget *parent)
    : QGLView(),
	m_object(nullptr),
	m_colorBox(_colorBox),
	m_selected(nullptr),
	m_operationAxis(0),
	m_hideXL(0),
	m_hideYL(0),
	m_hideZL(0),
	m_hideXR(_res-1),
	m_hideYR(_res-1),
	m_hideZR(_res-1),
	m_showHidden(true),
	m_mode(Mode::Set)
{
//	connect(_colorBox, SIGNAL(currentIndexChanged(QString &)),this, SLOT(colorChanged(QString &)));
	//setup options
	setOption(QGLView::ObjectPicking, true);
	setOption(QGLView::ShowPicking, false);
	glEnable(GL_BLEND);
	// Set up sizes
	m_res = _res;
	m_res_2 = m_res*m_res;
	m_res_3 = m_res_2*m_res;

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
	m_cubeData.resize(m_res_3);

	for(int i = 0; i < MAXUNDOSTEPS; ++i)
		m_cubeDataPrev[i].resize(m_res_3);
	m_prevPtr = 0;
	m_prevPtrBegin = 0;

	size_t index = 0;

    FOREACH
	{
		//build cube
		QGLBuilder builder;
		builder << QGL::Faceted;
		builder << QGLCube(0.44);//0.5
	
		m_cube[x][y][z] = builder.finalizedSceneNode();
	//	m_cube[ix][iy][iz]->setParent(&pickScene);
		m_pickScene.mainNode()->addNode(m_cube[x][y][z]);
				
		//arrange
		m_cube[x][y][z]->setPosition(QVector3D(m_offset+x*0.5, m_offset+y*0.5, m_offset+z*0.5));

		m_cubeData[index].setNode(m_cube[x][y][z]);
		m_cubeData[index].locX = x;
		m_cubeData[index].locY = y;
		m_cubeData[index].locZ = z;

		index++;
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
	}
}
 
void CubeView::setTex( unsigned int* _tex )
{
	size_t index = 0;
	FOREACH
	{
		unsigned int col = _tex[x + y*m_res + z*m_res*m_res];
		m_cubeData[index].setColor(col);
		m_cubeData[index].setState(col);

		index++;
	}
}

void CubeView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().rotate(45.0f, 0.0f, 0.0f, 0.0f);
	
	painter->setStandardEffect(QGL::StandardEffect::FlatColor);
	QArray<QVector3D> line; line.resize(2);
	line[0] = m_axisOffset;

	// axis
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

	size_t index = 0;

	FOREACH
	{
	//	QGLMaterial material;
	//	material.setColor
		
		painter->setColor(m_cubeData[index].m_color);
		 painter->setFaceColor(QGL::AllFaces, m_cubeData[index].m_color);
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
		index++;
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
	if(_real) return &m_cubeData[INDEX(locX, locY, locZ)]; 

	//broadcast to all neighbours
	if(locX+1 < m_res) m_cubeData[INDEX(locX+1, locY, locZ)].setState(true);
	if(locX-1 >= 0) m_cubeData[INDEX(locX-1, locY, locZ)].setState(true);
	if(locY+1 < m_res) m_cubeData[INDEX(locX, locY+1, locZ)].setState(true);
	if(locY-1 >= 0) m_cubeData[INDEX(locX, locY-1, locZ)].setState(true);
	if(locZ+1 < m_res) m_cubeData[INDEX(locX, locY, locZ+1)].setState(true);
	if(locZ-1 >= 0) m_cubeData[INDEX(locX, locY, locZ-1)].setState(true);

	//second try should pick the cube in the focus
	pickNode = dynamic_cast<QGLPickNode*>(objectForPoint(m_mPos));
	if(pickNode) 
	{
		m_object = (QGLSceneNode*)(pickNode->target()->parent());

		//recover the old condition
	if(locX+1 < m_res) m_cubeData[INDEX(locX+1, locY, locZ)].recoverState();
	if(locX-1 >= 0) m_cubeData[INDEX(locX-1, locY, locZ)].recoverState();
	if(locY+1 < m_res) m_cubeData[INDEX(locX, locY+1, locZ)].recoverState();
	if(locY-1 >= 0) m_cubeData[INDEX(locX, locY-1, locZ)].recoverState();
	if(locZ+1 < m_res) m_cubeData[INDEX(locX, locY, locZ+1)].recoverState();
	if(locZ-1 >= 0) m_cubeData[INDEX(locX, locY, locZ-1)].recoverState();

		//retrieve loc for choosen node
		locX = (m_object->x()-m_offset)/0.5;
		locY = (m_object->y()-m_offset)/0.5;
		locZ = (m_object->z()-m_offset)/0.5;

		return &m_cubeData[INDEX(locX, locY, locZ)];
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
	if(_e->modifiers() == Qt::KeyboardModifier::AltModifier)
		setOption(QGLView::ObjectPicking, false);

	QGLView::mousePressEvent(_e);

	m_mPos = _e->pos();
	//save currently selected voxel
	m_selected = getMouseFocus((_e->button() == Qt::LeftButton));
}

namespace{
	template<typename T>
	inline T sqr(T num) {return num * num;}
}

void CubeView::mouseReleaseEvent( QMouseEvent* _e )
{
	QGLView::mouseReleaseEvent(_e);
	m_mPos = _e->pos();

	//when object picking is disabled this is a rotation
	if(!(options() & QGLView::ObjectPicking)) 
	{
		setOption(QGLView::ObjectPicking, true);
		return;
	}

	std::function<void(Cube&)> processCube;

	Cube* focus;
	if(_e->button() == Qt::LeftButton || m_mode == Mode::Sphere)
		focus = getMouseFocus(true);
	else focus = getMouseFocus(false);

	//when there are not two selected do nothing
	if(!focus || !m_selected) return;

	//save previous state
	saveAsPrevious();

	if(_e->button() == Qt::LeftButton)
	{
		processCube = [](Cube& _cube){_cube.setState(false);};
	}
	else if(_e->button() == Qt::RightButton)
	{
		colorChanged(m_colorBox->currentText());
		switch(m_mode)
		{
		case Mode::Set:
			processCube =  [&](Cube& _cube){
				_cube.setColor(m_color);
				_cube.setState(true);
			};
			break;
		case Mode::Fill:
			processCube =  [&](Cube& _cube){
				if(_cube.getState()) return;
				_cube.setColor(m_color);
				_cube.setState(true);
			};
			break;
		case Mode::Sphere:
			float centerX = (m_selected->locX + focus->locX) / 2.f;
			float centerY = (m_selected->locY + focus->locY) / 2.f;
			float centerZ = (m_selected->locZ + focus->locZ) / 2.f;
			float radX = abs((m_selected->locX - focus->locX) / 2.f);
			radX = sqr(radX);
			float radY = abs((m_selected->locY - focus->locY) / 2.f);
			radY = sqr(radY);
			float radZ = abs((m_selected->locZ - focus->locZ) / 2.f);
			radZ = sqr(radZ);
			processCube = [=](Cube& _cube){
				if(sqr(_cube.locX-centerX)/radX  + sqr(_cube.locY-centerY)/radY + sqr(_cube.locZ-centerZ)/radZ <= 1.f )
				{
					_cube.setColor(m_color);
					_cube.setState(true);
				}
			};
			break;
		}
	}

	//get directions for the iteration
	int dx = focus->locX - m_selected->locX > 0 ? 1 : -1;
	int dy = focus->locY - m_selected->locY > 0 ? 1 : -1;
	int dz = focus->locZ - m_selected->locZ > 0 ? 1 : -1;
	//iterate througth the volume
	for(int ix = m_selected->locX; ix != focus->locX + dx; ix += dx)
		for(int iy = m_selected->locY; iy != focus->locY + dy; iy += dy)
			for(int iz = m_selected->locZ; iz != focus->locZ + dz; iz += dz)
				processCube(m_cubeData[INDEX(ix, iy, iz)]);
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

// ********************************************** //

void CubeView::keyPressEvent(QKeyEvent* e)
{
	this->setTitle(QString::fromStdString(std::to_string(e->key())));
	if(e->matches(QKeySequence::StandardKey::Undo))
	{
		//undo can only be down when some change happend
		if(m_prevPtr == m_prevPtrBegin) return;
		//retrieve previous state
		for(size_t i = 0; i < m_res_3; i++)
		{
			m_cubeData[i].setColor(m_cubeDataPrev[m_prevPtr][i].color);
			m_cubeData[i].setState(m_cubeDataPrev[m_prevPtr][i].state);
		}
		if(m_prevPtr == 0) m_prevPtr = 9;
		m_prevPtr--;

		//show changes
		update();
	}
	else if( e->key() == 88)//x
	{
		m_operationAxis = 0;	
	}
	else if( e->key() == 89)//y
	{
		m_operationAxis = 1;	
	}
	else if( e->key() == 90)//z
	{
		m_operationAxis = 2;	
	}
	else if( e->key() == 16777235)//arrow up
	{
		shiftCubes( int(m_operationAxis == 0), int(m_operationAxis == 1), int(m_operationAxis == 2) );
	}
	else if( e->key() == 16777237)//arrow down
	{
		shiftCubes( - int(m_operationAxis == 0), - int(m_operationAxis == 1), - int(m_operationAxis == 2) );
	}
	else if( e->key() == 16777234)//arrow left
	{
		hideCubesLeft( int(m_operationAxis == 0), int(m_operationAxis == 1), int(m_operationAxis == 2) );
	}
	else if( e->key() == 16777236)//arrow right
	{
		hideCubesRight(int(m_operationAxis == 0), int(m_operationAxis == 1), int(m_operationAxis == 2) );
	}
	else if( e->key() == 82)//r
	{
		rotateCubes(int(m_operationAxis == 0), int(m_operationAxis == 1), int(m_operationAxis == 2) );
	}
	else if( e->key() == 84 && e->modifiers() & Qt::KeyboardModifier::ControlModifier)
	{
		toggleHidden();
	}
	else if(e->key() == 78) //n
	{
		saveAsPrevious();

		int begin = (int)floor((float)m_res * 0.5f - 0.5f);
		int end = (int)ceil((float)m_res * 0.5f + 0.5f);

		colorChanged(m_colorBox->currentText());

		for(int ix = begin; ix < end; ix++)
			for(int iy = begin; iy < end; iy++)
				for(int iz = begin; iz < end; iz++)
				{
					m_cubeData[INDEX(ix,iy,iz)].setColor(m_color);
					m_cubeData[INDEX(ix,iy,iz)].setState(true);
				}
		// show
		update();
	}
	else if(e->key() == QKeySequence::Back)
	{
	}
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

// ***************************************************** //

void CubeView::changeCubes(unsigned int _color, unsigned int _newColor, bool _state)
{
	size_t index = 0;
	FOREACH
	{
		if(m_cubeData[index].m_colorOrg == _color)
		{
			m_cubeData[index].setColor(_newColor);
			m_cubeData[index].setState(_state);
		}

		index++;
	}
	//show changes
	update();
}

// ***************************************************** //
const std::string& CubeView::changeMode()
{
	m_mode = (Mode)((int)m_mode + 1);
	if(m_mode == Mode::Count) m_mode = Mode::Set;

	return ModeStrings[m_mode];
}

// ***************************************************** //

void CubeView::saveAsPrevious()
{
	m_prevPtr = (m_prevPtr+1) % MAXUNDOSTEPS;

	//when all buffers are used 
	if(m_prevPtr == m_prevPtrBegin) m_prevPtrBegin = (m_prevPtr+1) % 10;
	
	for(size_t i = 0; i < m_res_3; ++i)
	{
		m_cubeDataPrev[m_prevPtr][i].color = m_cubeData[i].m_colorOrg;
		m_cubeDataPrev[m_prevPtr][i].state = m_cubeData[i].getState();
	}
}

// ***************************************************** //

void CubeView::shiftCubes(int _x, int _y, int _z)
{
	saveAsPrevious();

	int offset = INDEX(_x, _y, _z);

	FOREACH
	{
		int i = INDEX(x, y, z);
		int newInd = i - offset;
		//origin is outside
		int dx = x - _x;
		int dy = y - _y;
		int dz = z - _z;


		if( dx < 0 || dx >= m_res || dy < 0 || dy >= m_res || dz < 0 || dz >= m_res )
		{
			m_cubeData[i].setState(false);
		}
		else
		{
			m_cubeData[i].setColor(m_cubeDataPrev[m_prevPtr][newInd].color);
			m_cubeData[i].setState(m_cubeDataPrev[m_prevPtr][newInd].state);
		}
	}

	update();
}

// ***************************************************** //

int newVal(int _rot, int _old, int _new)
	{
		while(_rot < 0) _rot += 4;

		switch (_rot)
		{
			case 0: return _old;
			case 1: return _new;
			case 2: return -_old;
			case 3: return -_new;
		}
	}

void CubeView::rotateCubes(int _x, int _y, int _z)
{
	saveAsPrevious();

	size_t i = 0;

	FOREACH
	{
		//every component value is changed by rotations around the two other axis 
		int index = INDEX(newVal(_z, newVal(_y, x, z), y), newVal(_z, newVal(_x, y, z), x), newVal(_y, newVal(_x, z, y), x));

		m_cubeData[i].setColor(m_cubeDataPrev[m_prevPtr][index].color);
		m_cubeData[i].setState(m_cubeDataPrev[m_prevPtr][index].state);

		i++;
	}

	update();
}

// ********************************************** //
void CubeView::hideCubesLeft(int _x, int _y, int _z)
{
	m_hideXL += _x;
	m_hideYL += _y;
	m_hideZL += _z;
	
	updateHidden();
}

void CubeView::hideCubesRight(int _x, int _y, int _z)
{
	m_hideXR -= _x;
	m_hideYR -= _y;
	m_hideZR -= _z;
	
	updateHidden();
}

void CubeView::updateHidden()
{
	FOREACH
	{
		int ind = INDEX(x,y,z);
		if(x >= m_hideXL && y >= m_hideYL && z >= m_hideZL &&
			x <= m_hideXR && y <= m_hideYR && z <= m_hideZR)
			m_cubeData[ind].setHidden(!m_cubeData[ind].getState());
		else m_cubeData[ind].setHidden(true);
	}
	update();
}

void CubeView::showHidden()
{
	m_hideXL = 0;
	m_hideYL = 0;
	m_hideZL = 0;
	m_hideXR = m_res-1;
	m_hideYR = m_res-1;
	m_hideZR = m_res-1;
	
	showAll();
}

void CubeView::showAll()
{
	FOREACH
	{
		int ind = INDEX(x,y,z);
		m_cubeData[ind].setHidden(!m_cubeData[ind].getState());
	}
	update();
}

void CubeView::toggleHidden()
{
	m_showHidden = !m_showHidden;

	if(m_showHidden)
		showAll();
	else
		updateHidden();
}