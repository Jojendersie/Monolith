#pragma once
#include "qglabstractscene.h"
class SceneManager :
	public QGLAbstractScene
{
public:
	SceneManager(QObject *parent = 0);
	~SceneManager(void);

	virtual QList<QObject *> objects() const;
	QGLSceneNode *mainNode() const { return m_rootNode; }
private:
	QGLSceneNode* m_rootNode;
};

