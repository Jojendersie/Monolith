#include "attributetable.h"
#include <QCloseEvent>

AttributeTable::AttributeTable(QWidget *parent, std::vector<Attribute>& _attributes)
	:QDialog(parent),
	m_attributes(_attributes)
{
	ui.setupUi(this);

	connect(ui.atrTable, SIGNAL(cellChanged ( int, int )), this,
            SLOT(cellChanged ( int, int )));

	setWindowTitle(QString::fromStdString("This is sparta!"));

	//add one to allow adding of other attributes
	ui.atrTable->setRowCount(m_attributes.size() + 1);
	ui.atrTable->setColumnCount(2);

	ui.atrTable->setColumnWidth(0, 200);

	QStringList header;
	header << "name" << "value";
	ui.atrTable->setHorizontalHeaderLabels(header);
    ui.atrTable->verticalHeader()->setVisible(false);

	for(int i = 0; i < m_attributes.size(); ++i)
	{
		QTableWidgetItem* tableItem = new QTableWidgetItem(QString::fromStdString(m_attributes[i].name));
		tableItem->setFlags(tableItem->flags() | Qt::ItemIsEditable);
		ui.atrTable->setItem(i, 0, tableItem);
		tableItem = new QTableWidgetItem(QString::number(m_attributes[i].value));
		tableItem->setFlags(tableItem->flags() | Qt::ItemIsEditable);
		ui.atrTable->setItem(i, 1, tableItem);
	}
}


AttributeTable::~AttributeTable(void)
{
}

void AttributeTable::cellChanged(int row, int column)
{
	int rowCount = ui.atrTable->rowCount();
	//last row got accessed
	if(row == rowCount - 1)
	{
		ui.atrTable->setRowCount(rowCount + 1);
		ui.atrTable->setItem(rowCount - 1, 1, new QTableWidgetItem(QString("0.0")));
	}
}

void AttributeTable::accept()
{
	//save table
	m_attributes.resize(0);

	for(int i = 0; i < ui.atrTable->rowCount() - 1; ++i)
	{
		Attribute atr;
		atr.name = ui.atrTable->item(i,0)->text().toStdString();
		atr.value = ui.atrTable->item(i,1)->text().toFloat();
		m_attributes.push_back(atr);
	}

	//close window
	QDialog::accept();
}