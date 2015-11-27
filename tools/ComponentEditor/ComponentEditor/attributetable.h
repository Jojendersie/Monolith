#pragma once
#include <QtWidgets/QDialog>
#include "ui_atrtable.h"
#include "voxel.h"

class AttributeTable : public QDialog
{
	Q_OBJECT
public:
	AttributeTable(QWidget *parent, std::vector<Attribute>& _attributes);
	~AttributeTable(void);

public slots:
	void accept() override;
private slots:
	void cellChanged ( int row, int column );
private:

	Ui::Dialog ui;
	std::vector<Attribute>& m_attributes;
};

