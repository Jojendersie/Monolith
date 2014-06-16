#include "componenteditor.h"
#include <QtWidgets/QApplication>
#include "cubeview.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setApplicationName("componentEditor");
    a.setApplicationVersion("0.1");

	ComponentEditor w;
	w.show();

/*	CubeView view(w.ui_comboBox);
	view.setHeight(512);
	view.setWidth(512);
	view.setPosition(w.pos().x()+w.width()+24,w.pos().y());
    view.show();*/

	return a.exec();
}
