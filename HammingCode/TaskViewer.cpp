#include "HammingCode.h"

TaskViewer::TaskViewer(QWidget* parent, HammingCode& mainClass) :
	QWidget(parent), mainClass(mainClass)
{
	taskLabel = new QLabel(parent);
	answerLabel = new QLabel(parent);
	table = new QTableWidget(2, 3, parent);
	answer = new QTableWidget(1, 1, parent);
	checkButton = new QPushButton(parent);
	verdictLabel = new QLabel(parent);
	verdictBox = new FocusWhellComboBox(parent);
	auto addItems = [](QTableWidget* table) {
		for (int i = 0; i < table->rowCount(); ++i) {
			for (int j = 0; j < table->columnCount(); ++j) {
				table->setItem(i, j, new QTableWidgetItem());
			}
		}
		};
	auto setTableOptions = [](QTableWidget* table) {
		table->verticalHeader()->setVisible(0);
		table->horizontalHeader()->setVisible(0);
		table->setSelectionMode(QAbstractItemView::NoSelection);
		table->setAutoScroll(0);
		table->setEditTriggers(QAbstractItemView::DoubleClicked);
		};
	addItems(table);
	addItems(answer);
	setTableOptions(table);
	setTableOptions(answer);
	for (int i = 0; i < table->rowCount(); ++i) {
		for (int j = 0; j < table->columnCount(); ++j) {
			table->item(i, j)->setFlags(Qt::ItemIsEnabled);
		}
	}
	taskLabel->setText("Чтобы увидеть графики, решите задание.");
	answerLabel->setText("Ответ:");
	verdictLabel->setText("Вердикт:");
	verdictBox->addItem("Нет ошибки");
	verdictBox->addItem("Одна ошибка");
	verdictBox->addItem("Чётная ошибка");
	verdictBox->addItem("Нечётная ошибка");
	table->item(0, 0)->setText("Последовательность");
	table->item(0, 1)->setText("Задача");
	table->item(0, 2)->setText("Тип");

	int tableHeight = 50;
	taskLabel->setGeometry(0, 0, parent->width(), tableHeight);
	table->setGeometry(0, taskLabel->y() + taskLabel->height(), parent->width(), 2 * tableHeight);
	answerLabel->setGeometry(0, table->y() + table->height() + 10, parent->width() / 2, tableHeight);
	verdictLabel->setGeometry
	(parent->width() / 2, answerLabel->y(), parent->width() / 2, answerLabel->height());
	verdictBox->setGeometry
	(verdictLabel->x(), verdictLabel->y() + verdictLabel->height(),
		verdictLabel->width(), verdictLabel->height());
	answer->setGeometry
	(0, answerLabel->y() + answerLabel->height(), parent->width() - verdictBox->width(), tableHeight);
	checkButton->setGeometry
	(0, answer->y() + answer->height() + 10, parent->width(), tableHeight);

	checkButton->setText("Проверить");

	answer->horizontalHeader()->setDefaultSectionSize(answer->width());
	answer->verticalHeader()->setDefaultSectionSize(tableHeight);
	table->horizontalHeader()->setDefaultSectionSize(parent->width() / 3);
	table->verticalHeader()->setDefaultSectionSize(tableHeight - 1);
	connect(answer, &QTableWidget::clicked, answer, QOverload<const QModelIndex&>::of(&QTableWidget::edit));
	connect(checkButton, SIGNAL(clicked()), this, SLOT(check()));
	connect(answer, &QTableWidget::itemChanged, this, &TaskViewer::answerChanged);
	hide();
}

TaskViewer::~TaskViewer()
{
	delete taskLabel;
	delete answerLabel;
	auto delTable = [](QTableWidget* table) {
		if (!table) return;
		for (int i = 0; i < table->rowCount(); ++i) {
			for (int j = 0; j < table->columnCount(); ++j) {
				delete table->item(i, j);
			}
		}
		delete table;
		};
	delTable(table);
	table = 0;
	delTable(answer);
	answer = 0;
}

void TaskViewer::show()
{
	taskLabel->show();
	answerLabel->show();
	table->show();
	answer->show();
	checkButton->show();
	if (manager.task[1] == "1" && manager.task[2] == "1") {
		verdictLabel->show();
		verdictBox->show();
	}
}

void TaskViewer::hide()
{
	taskLabel->hide();
	answerLabel->hide();
	table->hide();
	answer->hide();
	checkButton->hide();
	verdictLabel->hide();
	verdictBox->hide();
}

bool TaskViewer::newTask()
{
	if (hasTask) return 1;
	++task_num;
	if (task_num % 2 == 0) return 0;
	if (manager.hasTasks()) {
		hasTask = 1;
		manager.newTask();
		answer->item(0, 0)->setText("");
		verdictBox->setCurrentIndex(0);
		table->item(1, 0)->setText(QString::fromStdString(manager.task[0]));
		if (manager.task[1] == "1") {
			table->item(1, 1)->setText("Декодирование");
		}
		else {
			table->item(1, 1)->setText("Кодирование");
		}
		if (manager.task[2] == "1") {
			table->item(1, 2)->setText("Модифицированный");
		}
		else {
			table->item(1, 2)->setText("Не модифицированный");
		}
		return 1;
	}
	return 0;
}

void TaskViewer::answerChanged(QTableWidgetItem* item)
{
	item->setBackground(Qt::white);
}

void TaskViewer::check() {
	if (manager.task[2] == "1") {
		if (manager.checkAnswer(answer->item(0, 0)->text().toStdString(),
			manager.task[1] == "1" ?
			(TaskManager::ModifiedVerdict)verdictBox->currentIndex() : TaskManager::ModifiedVerdict::nonModified)) {
			hasTask = 0;
			// execute plot view
			mainClass.calculate_clicked();
		}
		else {
			goto WA;
		}
	}
	else {
		if (manager.checkAnswer(answer->item(0, 0)->text().toStdString(),
			TaskManager::ModifiedVerdict::nonModified)) {
			hasTask = 0;
			// execute plot view
			mainClass.calculate_clicked();
		}
		else {
			goto WA;
		}
	}
	return;
WA:
	answer->blockSignals(1);
	answer->item(0, 0)->setBackground(Qt::red);
	answer->blockSignals(0);
}