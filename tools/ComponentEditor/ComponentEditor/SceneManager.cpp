#include "SceneManager.h"


SceneManager::SceneManager(QObject *parent):
	QGLAbstractScene(parent),
	m_rootNode(new QGLSceneNode(this))
{
}


SceneManager::~SceneManager(void)
{
	delete m_rootNode;
}

QList<QObject *> SceneManager::objects() const
{
    QList<QGLSceneNode *> children = m_rootNode->allChildren();
    QList<QObject *> objects;
    for (int index = 0; index < children.size(); ++index)
        objects.append(children.at(index));
    return objects;
}
