/* cube ************************************************
 * cube represantation
 */

#include <qcolor>
#include <qglscenenode>


class Cube : public QObject
{
public:
	Cube(void);
	~Cube(void);
	void setNode(QGLSceneNode* _node){m_node = _node;};
	

	//sets the state permanently
	void setState(bool _state);
	bool getState() {return m_state;};
	//retrieves the previous state
	void recoverState(){setState(m_statePre);};

	//takes a YCbrCr value and sets the intern color
	void setColor(unsigned int _code){m_color = yCbCrToRGB(_code); m_colorOrg = _code;};
	QColor m_color;
	unsigned int m_colorOrg;

	//transoforms a YCbrCr uint to a rgb color
	static 	QColor yCbCrToRGB(unsigned int _code); 
private:
	QGLSceneNode* m_node;//pointer on the grafical and physical block
	bool m_state; //whether the block exists in the current voxel
	bool m_statePre;
	unsigned int m_material; //
};

