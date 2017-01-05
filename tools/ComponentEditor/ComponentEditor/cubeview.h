#ifndef CUBEVIEW_H
#define CUBEVIEW_H

#include <Qt3D/qglview>
#include <Qt3D/qgltexture2d>
#include "cube.h"
#include "attributetable.h" //actually only "voxel.h" is required
#include <vector>
#include <array>

#include "SceneManager.h"

class QGLSceneNode;
class QGLAbstractScene;
class QComboBox;

const int COMPSIZE = 3; 

using namespace std;

#define INDEX(x,y,z) (z)+m_res*(y)+m_res*m_res*(x)

enum Mode
{
	Set,
	Fill, 
	Sphere,
	Count
};

const std::array<std::string, Count> ModeStrings = 
{
	std::string("set"),
	std::string("fill"),
	std::string("Ellipsoid")
};


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
	unsigned int getCol(int _x, int _y, int _z) {return m_cubeData[INDEX(_x, _y, _z)].m_colorOrg;};
	bool getState(int _x, int _y, int _z) {return m_cubeData[INDEX(_x, _y, _z)].getState();};

	//changes the state and color of all cubes with the color _color
	void changeCubes(unsigned int _color, unsigned int _newColor = 0, bool _state = false);

	const std::string& changeMode();
	// makes all hidden cubes visible again
	// _reset should 
	void showHidden();
	void toggleHidden();
protected:
    void paintGL(QGLPainter *painter);
	void mouseMoveEvent ( QMouseEvent * e );
	void mousePressEvent ( QMouseEvent * e );
	void mouseReleaseEvent ( QMouseEvent * e );
	void keyPressEvent ( QKeyEvent* e);

private:
	//retrieves the cube focused by the mouse when _real == true
	//otherwise returns the block in the direction of the face under the cursor
	//when the face is a border of the voxel-cube that cube is returned instead
	Cube* getMouseFocus(bool _real = false);

	void saveAsPrevious();

	// shifts all cubes by the specified offset
	void shiftCubes(int _x, int _y, int _z);

	// rotates all cubes by the amount * 90 degree around the related axis
	void rotateCubes(int _x, int _y, int _z);

	// hides a cube layer in the specified direction
	void hideCubesLeft(int _x, int _y, int _z);
	void hideCubesRight(int _x, int _y, int _z);

	void updateHidden();
	void showAll();
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
	vector< Cube > m_cubeData;
	//data of previous state
	//vector< CubeStateData > m_cubeDataPrev;
	vector< CubeStateData > m_cubeDataPrev[10];
	size_t m_prevPtr;
	size_t m_prevPtrBegin;

	//position offset of the coordinate axis
	QVector3D m_axisOffset;

	//the currently displayed voxel
	Voxel* m_voxel;
	//tex resolution of the current voxel
	int m_res, m_res_2, m_res_3;

	//voxels hidden in each direction
	int m_hideXL, m_hideYL, m_hideZL;
	int m_hideXR, m_hideYR, m_hideZR;
	bool m_showHidden;

	//requiered to allow picking
	SceneManager  m_pickScene;
    
	//color selected in the combobox
	unsigned int m_color;
	QComboBox* m_colorBox;

	//axis selected for shifting or rotating operations
	int m_operationAxis;
	Mode m_mode;
public slots:
	void objectPicked();
	void colorChanged(QString& _s);
};

#endif