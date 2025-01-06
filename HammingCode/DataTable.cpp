#include "HammingCode.h"
DataTable::DataTable(int iterations, bool modified)
{
	i = 4;
	table = new QTableWidget(iterations + 4, 9 + modified, 0);
	table->verticalHeader()->setVisible(0);
	table->verticalHeader()->setDefaultSectionSize(100);
	table->horizontalHeader()->setVisible(0);
	table->horizontalHeader()->setDefaultSectionSize(200);
	table->setEditTriggers(QAbstractItemView::DoubleClicked);
	for (int i = 0; i < table->rowCount(); ++i) {
		for (int j = 0; j < table->columnCount(); ++j) {
			table->setItem(i, j, new QTableWidgetItem());
		}
	}
	auto setItem = [&](int i, int j, const QString& s) {
		//table->item(i, j)->setFlags(Qt::ItemIsEnabled);
		table->item(i, j)->setText(s);
		};
	setItem(0, 0, "Доверительный интервал");
	setItem(1, 0, "Минимум:");
	setItem(1, 2, "Максимум:");
	setItem(1, 4, "Уровень доверия:");
	setItem(2, 0, "Эксперименты");
	setItem(3, 0, "Номер итерации");
	setItem(3, 1, "Принятая последовательность");
	setItem(3, 2, "Восстановленная последовательность");
	setItem(3, 3, "Декодированная последовательность");
	setItem(3, 4, "Количество искажённых помехой бит");
	setItem(3, 5, "Количество верно исправленных бит в закодированной последовательности");
	setItem(3, 6, "Количество верно исправленных бит в декодированной последовательности");
	setItem(3, 7, "Количество ошибок в декодированной последовательности");
	setItem(3, 8, "Процент ошибок в декодированной последовательности");
	if (modified) setItem(3, 9, "Количество верных срабатываний проверочного бита");
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < table->columnCount(); ++j) {
			table->item(i, j)->setFlags(Qt::ItemIsEnabled);
		}
	}
	for (int i = 4; i < table->rowCount(); ++i) {
		for (int j = 0; j < table->columnCount(); ++j) {
			table->item(i, j)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		}
	}
	table->hide();
}

void DataTable::addRow(const std::vector<std::string>& data)
{
	int n = data.size();
	for (int j = 0; j < n; ++j) {
		table->item(i, j)->setText(QString::fromStdString(data[j]));
	}
	++i;
}

void DataTable::setTrustLevel(double min, double max, double lvl)
{
	table->item(1, 1)->setText(QString::number(min, 103, 2));
	table->item(1, 3)->setText(QString::number(max, 103, 2));
	table->item(1, 5)->setText(QString::number(lvl, 103, 2));
}

DataTable::~DataTable()
{
	for (int i = 0; i < table->rowCount(); ++i) {
		for (int j = 0; j < table->columnCount(); ++j) {
			delete table->item(i, j);
		}
	}
	delete table;
}

void DataTable::show()
{
	table->show();
}

void DataTable::hide()
{
	table->hide();
}
