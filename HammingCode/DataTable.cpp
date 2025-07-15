#include "HammingCode.h"
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include "IterationResultTableModel.h"

DataTable::DataTable()
{
	view = new QTableView(this);
	
	view->setAlternatingRowColors(true);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	view->horizontalHeader()->setMinimumSectionSize(200);
	view->horizontalHeader()->setFixedHeight(60);
	view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	view->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	view->verticalHeader()->setDefaultSectionSize(20);

	auto layout = new QVBoxLayout(this);
	layout->addWidget(view);
	layout->setContentsMargins(0, 0, 0, 0);
	view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

	view->setSelectionMode(QAbstractItemView::ContiguousSelection);
	view->setSelectionBehavior(QAbstractItemView::SelectItems);

	auto* copyAction = new QAction(tr("Copy"), this);
	copyAction->setShortcut(QKeySequence::Copy);
	connect(copyAction, &QAction::triggered, this, &DataTable::copySelectionToClipboard);
	view->addAction(copyAction);
	view->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void DataTable::resizeToContentAndCenter() {
	auto screen = QGuiApplication::primaryScreen();
	auto available = screen->availableGeometry();
	
	view->resizeRowsToContents();
	auto size = view->sizeHint();
	if (size.width() > available.width()) size.setWidth(available.width());
	if (size.height() > available.height()) size.setHeight(available.height());
	resize(size);

	this->move(available.center() - rect().center());
}

void DataTable::createModel(std::vector<IterationResult>& results, bool isModifiedCode)
{
	auto old = view->model();
	model = new IterationResultTableModel(results, isModifiedCode);
	view->setModel(model);
	delete old;
}

void DataTable::copySelectionToClipboard()
{
	QModelIndexList indexes = view->selectionModel()->selectedIndexes();

	QString selectedText;
	QModelIndex previous = indexes.first();
	foreach(const QModelIndex & current, indexes)
	{
		QVariant data = model->data(current);
		QString text = data.toString();
		if (current.row() != previous.row()) {
			selectedText.append('\n');
		}
		else if (!selectedText.isEmpty()) {
			selectedText.append('\t');
		}
		selectedText.append(text);
		previous = current;
	}

	QGuiApplication::clipboard()->setText(selectedText);
}