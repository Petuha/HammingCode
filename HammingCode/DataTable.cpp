#include "HammingCode.h"
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>

DataTable::DataTable(int iterations, bool modified)
{
	i = 4;
	table = new QTableWidget(iterations + 4, 9 + modified, this);
	table->setAlternatingRowColors(true);
	table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	table->verticalHeader()->setVisible(0);
	table->horizontalHeader()->setVisible(0);
	table->horizontalHeader()->setDefaultSectionSize(200);
	table->setEditTriggers(QAbstractItemView::DoubleClicked);
	for (int i = 0; i < table->rowCount(); ++i) {
		for (int j = 0; j < table->columnCount(); ++j) {
			table->setItem(i, j, new QTableWidgetItem());
		}
	}
	auto setItem = [&](int i, int j, const QString& s, bool bold = true) {
		auto item = table->item(i, j);
		item->setText(s);

		QFont font = item->font();
		font.setBold(true);
		item->setFont(font);
	};
	setItem(0, 0, "ДОВЕРИТЕЛЬНЫЙ ИНТЕРВАЛ");
	table->setSpan(0, 0, 1, table->columnCount());
	setItem(1, 0, "Минимум:");
	setItem(1, 2, "Максимум:");
	setItem(1, 4, "Уровень доверия:");
	setItem(2, 0, "ЭКСПЕРИМЕНТЫ");
	table->setSpan(2, 0, 1, table->columnCount());
	setItem(3, 0, "Номер итерации");
	setItem(3, 1, "Принятая последовательность");
	setItem(3, 2, "Восстановленная последовательность");
	setItem(3, 3, "Декодированная последовательность");
	setItem(3, 4, "Количество искажённых помехой бит");
	setItem(3, 5, "Количество верно исправленных бит в закодированной последовательности");
	setItem(3, 6, "Количество верно исправленных бит в декодированной последовательности");
	setItem(3, 7, "Количество ошибок в декодированной последовательности");
	setItem(3, 8, "Доля ошибок в декодированной последовательности");
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

	auto layout = new QVBoxLayout(this);
	layout->addWidget(table);
	layout->setContentsMargins(0, 0, 0, 0);
	table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
}

void DataTable::addRow(const std::vector<std::string>& data)
{
	int n = data.size();
	for (int j = 0; j < n; ++j) {
		table->item(i, j)->setText(QString::fromStdString(data[j]));
	}
	++i;
}

void DataTable::resizeToContentAndCenter() {
	auto screen = QGuiApplication::primaryScreen();
	auto available = screen->availableGeometry();
	
	table->resizeRowsToContents();
	auto size = table->sizeHint();
	if (size.width() > available.width()) size.setWidth(available.width());
	if (size.height() > available.height()) size.setHeight(available.height());
	resize(size);

	this->move(available.center() - rect().center());
}

void DataTable::setTrustLevel(double min, double max, double lvl)
{
	table->item(1, 1)->setText(QString::number(min, 103, 2));
	table->item(1, 3)->setText(QString::number(max, 103, 2));
	table->item(1, 5)->setText(QString::number(lvl, 103, 2));
}