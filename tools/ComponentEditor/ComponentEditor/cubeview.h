#ifndef CUBEVIEW_H
#define CUBEVIEW_H

#include <Qt3D/qglview>
#include <Qt3D/qgltexture2d>
#include "cube.h"
#include "voxel.h"
#include <vector>

#include "SceneManager.h"

class QGLSceneNode;
class QGLAbstractScene;
class QComboBox;

const int COMPSIZE = 3; 

using namespace std;

class CubeView : public QGLView
{
    Q_OBJECT
public:
	//_colorBox for direct coloracess
	CubeView(QComboBox* _colorBox, int _res, QWidget *parent = 0);
    ~CubeView();

	//sets the modell up to reflect a specific texture
	void setTex( unsigned int* _tex );

	//returns the color of the cube as YCbCr
	unsigned int getCol(int _x, int _y, int _z) {return m_cubeData[_x][_y][_z]->m_colorOrg;};
	bool getState(int _x, int _y, int _z) {return m_cubeData[_x][_y][_z]->getState();};

	//changes the state and color of all cubes with the color _color
	void changeCubes(unsigned int _color, unsigned int _newColor = 0, bool _state = false);
protected:
    void paintGL(QGLPainter *painter);
	void mouseMoveEvent ( QMouseEvent * e );
	void mousePressEvent ( QMouseEvent * e );
	void mouseReleaseEvent ( QMouseEvent * e );

private:
	//retrieves the cube focused by the mouse when _real == true
	//otherwise returns the block in the direction of the face under the cursor
	//when the face is a border of the voxel-cube that cube is returned instead
	Cube* getMouseFocus(bool _real = false);

	//offset so that the center of the voxel is in the cam center
	float m_offset;

	//choosen cube for volume selection
	Cube* m_selected;

	//mouse pos of an event happening
	QPoint m_mPos;
	QGLSceneNode* m_object;
	//grafical representation
    vector<vector<vector<QGLSceneNode*> > > m_cube;
	//data for single cubes
	vector<vector<vector<Cube*> > > m_cubeData;

	//position offset of the coordinate axis
	QVector3D m_axisOffset;

	//the currently displayed voxel
	Voxel* m_voxel;
	//tex resolution of the current voxel
	int m_res;
	//requiered to allow picking
	SceneManager  m_pickScene;
    
	//color selected in the combobox
	unsigned int m_color;
	QComboBox* m_colorBox;

public slots:
	void objectPicked();
	void colorChanged(QString& _s);
};

#endif