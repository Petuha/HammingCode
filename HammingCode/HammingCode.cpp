#include "HammingCode.h"
#include <vector>

template<class T>
void setWidth(T* object, int w) {
	object->setGeometry(object->geometry().x(), object->geometry().y(), w, object->geometry().height());
}
template<class T>
void setHeight(T* object, int h) {
	object->setGeometry(object->geometry().x(), object->geometry().y(), object->geometry().width(), h);
}
template<class T>
void setX(T* object, int x) {
	object->setGeometry(x, object->geometry().y(), object->geometry().width(), object->geometry().height());
}
template<class T>
void setY(T* object, int y) {
	object->setGeometry(object->geometry().x(), y, object->geometry().width(), object->geometry().height());
}

QPixmap htmlText(const QString& text) {
	QTextDocument doc;
	doc.setHtml(text);
	doc.setTextWidth(doc.size().width());
	QPixmap pixmap(doc.size().width(), doc.size().height());
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	doc.drawContents(&painter);
	return pixmap;
}

HammingCode::HammingCode(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	int windowH = ui.centralWidget->parentWidget()->geometry().height();
	int windowW = ui.centralWidget->parentWidget()->geometry().width();

	// TODO make proper layout (preferrably in QtDesigner) and allow resizing
	ui.centralWidget->parentWidget()->setFixedSize(windowW, windowH);

	// Table labels
	label[0] = new QLabel("Эксперимент", ui.centralWidget);
	label[1] = new QLabel("Код Хэмминга", ui.centralWidget);
	label[2] = new QLabel("Сигнал", ui.centralWidget);
	label[3] = new QLabel("Помеха", ui.centralWidget);

	// Table params
	tableParams[1] = new QTableWidget(3, 2, ui.centralWidget);
	tableParams[2] = new QTableWidget(5, 2, ui.centralWidget);
	tableParams[3] = new QTableWidget(7, 2, ui.centralWidget);
	tableParams[0] = new QTableWidget(1, 2, ui.centralWidget);
	for (int param = 0; param < tableN; ++param) {
		for (int i = 0; i < tableParams[param]->rowCount(); ++i) {
			for (int j = 0; j < tableParams[param]->colorCount(); ++j) {
				tableParams[param]->setItem(i, j, new QTableWidgetItem());
			}
		}
		for (int i = 0; i < tableParams[param]->rowCount(); ++i) {
			tableParams[param]->item(i, 0)->setFlags(Qt::ItemIsEnabled);
		}
		tableParams[param]->verticalHeader()->setVisible(0);
		tableParams[param]->horizontalHeader()->setVisible(0);
		tableParams[param]->setSelectionMode(QAbstractItemView::NoSelection);
		tableParams[param]->setAutoScroll(0);
		tableParams[param]->setEditTriggers(QAbstractItemView::DoubleClicked);
	}
	auto setTableParamsLabel = [&]() {
		for (int i = 0; i < tableN; ++i) {
			tableParams[i]->horizontalHeader()->setDefaultSectionSize(150);
		}
		int tableW = tableParams[1]->horizontalHeader()->defaultSectionSize();
		int tableH = tableParams[1]->verticalHeader()->defaultSectionSize();
		int x = 10; // От балды
		int y = 0;
		auto setTableParams = [&](int i) {
			tableParams[i]->setGeometry(x, y,
				tableW * tableParams[i]->columnCount() + 2,
				tableH * tableParams[i]->rowCount() + 2);
			};
		auto setLabel = [&](int i) {
			setX(label[i], x);
			setY(label[i], y);
			};
		for (int i = 0; i < tableN; ++i) {
			setLabel(i);
			y += label[i]->geometry().height();
			setTableParams(i);
			y += tableParams[i]->geometry().height();
		}
		};
	setTableParamsLabel();

	// Set table values

	// Hamming values
	tableParams[1]->item(0, 0)->setText("Последовательность");
	tableParams[1]->item(1, 0)->setText("Размер блока");
	tableParams[1]->item(2, 0)->setText("Модифицированный");

	// Signal values
	tableParams[2]->item(0, 0)->setText("Метод преобразования");
	tableParams[2]->item(1, 0)->setText("Шаг дискретизации");
	tableParams[2]->item(2, 0)->setText("Амплитуда");
	tableParams[2]->item(3, 0)->setText("Точек на бит");
	tableParams[2]->item(4, 0)->setText("Полярность");

	// Noise values
	{
		QSize cellSize;
		cellSize.setWidth(tableParams[1]->horizontalHeader()->defaultSectionSize());
		cellSize.setHeight(tableParams[1]->verticalHeader()->defaultSectionSize());
		tableParams[3]->setIconSize(cellSize);
	}
	tableParams[3]->item(0, 0)->setIcon(htmlText("<math>t</math>"));
	tableParams[3]->item(1, 0)->setIcon(htmlText("<math>&Delta;t</math>"));
	tableParams[3]->item(2, 0)->setIcon(htmlText("<math>&nu;</math>"));
	tableParams[3]->item(3, 0)->setIcon(htmlText("<math>&Delta;&nu;</math>"));
	tableParams[3]->item(4, 0)->setIcon(htmlText("<math>Полярность</math>"));
	tableParams[3]->item(5, 0)->setIcon(htmlText("<math>a</math>"));
	tableParams[3]->item(6, 0)->setIcon(htmlText("<math>&Delta;a</math>"));

	// Experiment values
	tableParams[0]->item(0, 0)->setText("Число итераций");

	// Hamming QCombos
	modifiedBox = new FocusWhellComboBox(ui.centralWidget);
	modifiedBox->addItem("Нет");
	modifiedBox->addItem("Да");
	tableParams[1]->setCellWidget(2, 1, modifiedBox);

	// Signal Qcombos
	signalMethodBox = new FocusWhellComboBox(ui.centralWidget);
	signalMethodBox->addItem("NRZ");
	signalMethodBox->addItem("Манчестерский");
	signalMethodBox->addItem("RZ");
	signalMethodBox->addItem("AMI");
	tableParams[2]->setCellWidget(0, 1, signalMethodBox);

	signalPolarBox = new FocusWhellComboBox(ui.centralWidget);
	signalPolarBox->addItem("Прямая");
	signalPolarBox->addItem("Инвертированная");
	tableParams[2]->setCellWidget(4, 1, signalPolarBox);

	// Noise Qcombos
	noisePolarBox = new FocusWhellComboBox(ui.centralWidget);
	noisePolarBox->addItem("Однополярная");
	noisePolarBox->addItem("Биполярная");
	tableParams[3]->setCellWidget(4, 1, noisePolarBox);

	// Calculate button
	setX(ui.calculate, tableParams[3]->x());
	setY(ui.calculate, tableParams[3]->y() + tableParams[3]->height() + 10);
	setWidth(ui.calculate, tableParams[3]->width());
	setHeight(ui.calculate, 50);

	// Trust Interval Table
	{
		resultLabel = new QLabel(ui.centralWidget);
		setX(resultLabel, ui.calculate->x());
		setY(resultLabel, ui.calculate->y() + ui.calculate->height() + 10);
		setWidth(resultLabel, tableParams[0]->width());
		resultLabel->setText("Результаты:");
		resultLabel->hide();

		resultTable = new QTableWidget(5, 2, ui.centralWidget);
		setX(resultTable, resultLabel->x());
		setY(resultTable, resultLabel->y() + resultLabel->height());
		setWidth(resultTable, tableParams[0]->width());
		setHeight(resultTable,
			tableParams[0]->verticalHeader()->defaultSectionSize() * resultTable->rowCount() + 2
		); // this is fucked up, this whole window really needs some layouts
		resultTable->verticalHeader()->setVisible(0);
		resultTable->horizontalHeader()->setVisible(0);
		resultTable->horizontalHeader()->setDefaultSectionSize(tableParams[0]->horizontalHeader()->defaultSectionSize());

		auto setRow = [&](int i, const char* s, QTableWidgetItem** itemPtr) {
			auto item = new QTableWidgetItem(s);
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			resultTable->setItem(i, 0, item);

			item = new QTableWidgetItem();
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			resultTable->setItem(i, 1, item);

			*itemPtr = item;
		};
		auto setHeading = [&](int i, const char* s) {
			auto item = new QTableWidgetItem(s);
			item->setBackground(QBrush(QColor(245, 245, 245)));
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			resultTable->setItem(i, 0, item);
			resultTable->setSpan(i, 0, 1, 2);
		};
		setRow(0, "Закодированная послед.:", &encodedItem);
		setHeading(1, "Доверительный интервал");
		setRow(2, "Минимум", &trustIntervalMinimumItem);
		setRow(3, "Максимум", &trustIntervalMaximumItem);
		setRow(4, "Уровень доверия", &trustIntervalLevelItem);
		resultTable->hide();
	}

	// Plot Info Selectors
	plotSignalInfo = new QLabel(ui.centralWidget);
	plotSignalInfo->setText("Сигнал:");
	plotSignalInfo->hide();

	plotSignalSelector = new FocusWhellComboBox(ui.centralWidget);
	plotSignalSelector->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	plotSignalSelector->addItem("Отправляемый");
	plotSignalSelector->addItem("Принимаемый");
	plotSignalSelector->hide();

	plotErrorInfo = new QLabel(ui.centralWidget);
	plotErrorInfo->setText("Ошибка:");
	plotErrorInfo->hide();

	plotErrorSelector = new FocusWhellComboBox(ui.centralWidget);
	plotErrorSelector->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	plotErrorSelector->addItem("Наибольшая");
	plotErrorSelector->addItem("Наименьшая");
	plotErrorSelector->addItem("Медианная");
	plotErrorSelector->addItem("Наибольшая верно исправленная");
	plotErrorSelector->addItem("Выбранная итерация");
	plotErrorSelector->hide();

	plotIterationInfo = new QLabel(ui.centralWidget);
	plotIterationInfo->setText("Итерация:");
	plotIterationInfo->hide();

	plotIterationSelector = new QSpinBox(ui.centralWidget);
	plotIterationSelector->setMinimum(1);
	plotIterationSelector->hide();

	copyPlotToClipboard = new QPushButton(ui.centralWidget);
	copyPlotToClipboard->setText("Копировать в буфер");
	copyPlotToClipboard->hide();

	showTableButton = new QPushButton(ui.centralWidget);
	showTableButton->setText("Показать таблицу итераций");
	showTableButton->hide();

	plotInfoWidget = new QWidget(ui.centralWidget);
	setX(plotInfoWidget, tableParams[0]->x() + tableParams[0]->width() + 10);
	setY(plotInfoWidget, tableParams[0]->y());
	setWidth(plotInfoWidget, windowW - plotInfoWidget->x());
	setHeight(plotInfoWidget, plotErrorSelector->height());

	// Plot Info Area
	plotInfoLayout = new QHBoxLayout(plotInfoWidget);
	plotInfoLayout->setContentsMargins(0, 0, 0, 0);
	plotInfoLayout->setAlignment(Qt::AlignLeft);

	plotInfoLayout->addWidget(plotSignalInfo);
	plotInfoLayout->addWidget(plotSignalSelector);
	plotInfoLayout->addWidget(plotErrorInfo);
	plotInfoLayout->addWidget(plotErrorSelector);
	plotInfoLayout->addWidget(plotIterationInfo);
	plotInfoLayout->addWidget(plotIterationSelector);
	plotInfoLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	plotInfoLayout->addWidget(copyPlotToClipboard);
	plotInfoLayout->addWidget(showTableButton);

	// Plot Layout
	plotWidget = new QWidget(ui.centralWidget);
	plotWidget->setGeometry(
		plotInfoWidget->x(),
		plotInfoWidget->y() + plotInfoWidget->height(),
		windowW - plotInfoWidget->x(),
		windowH - (plotInfoWidget->y() + plotInfoWidget->height()));
	plotLayout = new QHBoxLayout(plotWidget);
	plotLayout->setContentsMargins(0, 0, 0, 0);

	// connects
	connect(plotErrorSelector, &FocusWhellComboBox::currentIndexChanged, this, &HammingCode::plotChanged);
	connect(plotSignalSelector, &FocusWhellComboBox::currentIndexChanged, this, &HammingCode::plotChanged);
	connect(plotIterationSelector, &QSpinBox::editingFinished, this, &HammingCode::onPlotIterationChanged);
	connect(copyPlotToClipboard, SIGNAL(clicked()), this, SLOT(copyClicked()));
	connect(showTableButton, SIGNAL(clicked()), this, SLOT(showTableClicked()));
	for (int i = 0; i < tableN; ++i) {
		connect(tableParams[i], &QTableWidget::clicked, tableParams[i],
			QOverload<const QModelIndex&>::of(&QTableWidget::edit));
		connect(tableParams[i], &QTableWidget::itemChanged, this, &HammingCode::itemChanged);
	}

	// hide status bar
	this->setStatusBar(0);

	// task viewer
	taskWidget = new QWidget(ui.centralWidget);
	taskWidget->hide();
	setWidth(taskWidget, 500);
	setHeight(taskWidget, 500);
	setX(taskWidget, plotWidget->x() + (plotWidget->width() - taskWidget->width()) / 2);
	setY(taskWidget, plotWidget->y() + (plotWidget->height() - taskWidget->height()) / 2);
	task = new TaskViewer(taskWidget, *this);

	// set default params
	auto setDeafaultTableParams = [](QTableWidget* table, int i, int j) {
		for (i; i < j; ++i) {
			table->item(i, 1)->setText("1");
		}
		};
	setDeafaultTableParams(tableParams[0], 0, 1);
	setDeafaultTableParams(tableParams[1], 0, 2);
	setDeafaultTableParams(tableParams[2], 1, 4);
	setDeafaultTableParams(tableParams[3], 0, 4);
	setDeafaultTableParams(tableParams[3], 5, tableParams[3]->rowCount());
	tableParams[2]->item(3, 1)->setText("2");
	tableParams[1]->item(0, 1)->setText("101010");
	tableParams[1]->item(1, 1)->setText("6");

	this->unlockTables();
}


HammingCode::~HammingCode()
{
}

void HammingCode::showTableClicked()
{
	if (dataTable) {
		// dirty fix for a bug: 
		// when the same table is showed again, the view lags on big tables
		// as no fetching is done and all the data is loaded at once
		dataTable->createModel(handler.iterationResults, handler.modified);
		dataTable->show();
		dataTable->resizeToContentAndCenter();
	}
}

void HammingCode::copyClicked()
{
	const int TARGET_WIDTH = 1920;
	const int TARGET_HEIGHT = 1080;

	int width = plot->width();
	int height = plot->height();
	double ratio = width < height ? static_cast<double>(TARGET_WIDTH) / width : static_cast<double>(TARGET_HEIGHT) / height;

	QImage image(width * ratio, height * ratio, QImage::Format_ARGB32);
	QPainter painter(&image);
	QwtPlotRenderer renderer;
	renderer.render(plot, &painter, image.rect());
	painter.end();

	QClipboard* clipboard = QGuiApplication::clipboard();
	if (!clipboard) return;
	clipboard->setImage(image);
}

void HammingCode::onPlotIterationChanged() {
	plotChanged(0);
}

void HammingCode::plotChanged(int _ = 0)
{
	if (plot == nullptr) return;

	plotErrorInfo->hide();
	plotErrorSelector->hide();
	plotIterationInfo->hide();
	plotIterationSelector->hide();
 	
	std::vector<Dot>* points;
	std::vector<Dot> generatedPoints;
	switch (plotSignalSelector->currentIndex()) {
	// Sent
	case 0:
		points = &handler.sent;
		break;
	// Received
	case 1:
		plotErrorInfo->show();
		plotErrorSelector->show();

		switch (plotErrorSelector->currentIndex()) {
		case 0:
			points = &handler.receivedMaxError;
			break;
		case 1:
			points = &handler.receivedMinError;
			break;
		case 2:
			points = &handler.receivedMedianError;
			break;
		case 3:
			points = &handler.receivedMaxCorrectedError;
			break;
		case 4:
			plotIterationInfo->show();
			plotIterationSelector->show();
			generatedPoints = handler.getReveicedOnIteration(plotIterationSelector->value() - 1);
			points = &generatedPoints;
			break;
		default:
			return;
		}
		break;
	default:
		return;
	}

	QPolygonF polygon;
	for (auto& p : *points) {
		polygon << QPointF(p.x, p.y);
	}
	curve->setSamples(polygon);
}

void HammingCode::itemChanged(QTableWidgetItem* item)
{
	item->setBackground(Qt::white);
}

void HammingCode::setTablesEnabled(bool flag)
{
	for (int t = 0; t < tableN; ++t) {
		tableParams[t]->setEnabled(flag);
	}
	modifiedBox->setEnabled(flag);
	noisePolarBox->setEnabled(flag);
	signalMethodBox->setEnabled(flag);
	signalPolarBox->setEnabled(flag);
}

void HammingCode::lockTables()
{
	setTablesEnabled(0);
}

void HammingCode::unlockTables()
{
	setTablesEnabled(1);
}

void HammingCode::calculate_clicked()
{
	// passed parameters check
	auto checkParams = [&]() -> bool {
		auto correctInt = [](const QString& s) -> bool {
			if (!s.size()) return 0; // empty string
			bool start = 1;
			for (QChar c : s) {
				if (c < '0' || c > '9') {
					if (!(start && c == '-')) return 0; // minus can be only at start
				}
				start = 0;
			}
			return 1;
			};
		auto correctDouble = [](const QString& s) -> bool {
			if (!s.size()) return 0; // empty string
			if (*s.begin() == '.' || *s.rbegin() == '.') return 0; // can't start or end with dot
			bool hasDot = 0, start = 1;
			for (QChar c : s) {
				if (c < '0' || c > '9') { // not digit
					if (c == '.') { // 1 dot allowed
						if (hasDot) return 0; // 2 dots not allowed
						hasDot = 1;
					}
					else if (c == '-') {
						if (!start) return 0; // minus can be only at start
					}
					else return 0;
				}
				start = 0;
			}
			return 1;
			};
		auto correctBits = [](const QString& s) -> bool {
			if (!s.size()) return 0; // empty string
			for (QChar c : s) {
				if (c != '0' && c != '1') return 0;
			}
			return 1;
			};
		auto positiveInteger = [&](const QString& s) -> bool {
			if (!correctInt(s)) return 0;
			return s.toInt() > 0;
			};
		auto nonNegativeInteger = [&](const QString& s) -> bool {
			if (!correctInt(s)) return 0;
			return s.toInt() >= 0;
			};
		auto positiveDouble = [&](const QString& s) -> bool {
			if (!correctDouble(s)) return 0;
			return s.toDouble() > 0;
			};
		auto nonNegativeDouble = [&](const QString& s) -> bool {
			if (!correctDouble(s)) return 0;
			return s.toDouble() >= 0;
			};
		auto correctChunkSize = [&](const QString& s, int bitsSize) -> bool {
			if (!positiveInteger(s)) return 0;
			return bitsSize % s.toInt() == 0;
			};

		if (!positiveInteger(tableParams[0]->item(0, 1)->text())) {
			tableParams[0]->item(0, 1)->setBackground(Qt::red);
			return 1;
		}

		if (!correctBits(tableParams[1]->item(0, 1)->text())) {
			tableParams[1]->item(0, 1)->setBackground(Qt::red);
			return 1;
		}

		if (!correctChunkSize(tableParams[1]->item(1, 1)->text(), tableParams[1]->item(0, 1)->text().size())) {
			tableParams[1]->item(1, 1)->setBackground(Qt::red);
			return 1;
		}

		auto checkTablePositiveDouble = [&](int ti, int i, int j) -> bool {
			for (i; i < j; ++i) {
				if (!positiveDouble(tableParams[ti]->item(i, 1)->text())) {
					tableParams[ti]->item(i, 1)->setBackground(Qt::red);
					return 1;
				}
			}
			return 0;
			};
		auto checkTableNonNegativeDouble = [&](int ti, int i, int j) -> bool {
			for (i; i < j; ++i) {
				if (!nonNegativeDouble(tableParams[ti]->item(i, 1)->text())) {
					tableParams[ti]->item(i, 1)->setBackground(Qt::red);
					return 1;
				}
			}
			return 0;
			};
		auto checkTableNonNegativeInteger = [&](int ti, int i, int j) -> bool {
			for (i; i < j; ++i) {
				if (!nonNegativeInteger(tableParams[ti]->item(i, 1)->text())) {
					tableParams[ti]->item(i, 1)->setBackground(Qt::red);
					return 1;
				}
			}
			return 0;
			};
		auto checkNoiseParams = [&]() -> bool {
			for (int i = 5, parity = 0; i < tableParams[3]->rowCount(); ++i, ++parity) {
				if (parity % 2 == 0) {
					if (!correctDouble(tableParams[3]->item(i, 1)->text())) {
						tableParams[3]->item(i, 1)->setBackground(Qt::red);
						return 1;
					}
				}
				else {
					if (!nonNegativeDouble(tableParams[3]->item(i, 1)->text())) {
						tableParams[3]->item(i, 1)->setBackground(Qt::red);
						return 1;
					}
				}
			}
			return 0;
			};
		auto checkDotsPerImpuls = [&]() -> bool {
			if (!correctInt(tableParams[2]->item(3, 1)->text())) {
				tableParams[2]->item(3, 1)->setBackground(Qt::red);
				return 1;
			}
			int x = tableParams[2]->item(3, 1)->text().toInt();
			if ((conversionMethod)signalMethodBox->currentIndex() == conversionMethod::MANCH) {
				if (x >= 4 && x % 2 == 0) return 0;
			}
			else {
				if (x >= 2) return 0;
			}
			tableParams[2]->item(3, 1)->setBackground(Qt::red);
			return 1;
			};

		if (checkTablePositiveDouble(2, 1, 3)) return 1;
		if (checkDotsPerImpuls()) return 1;
		if (checkTableNonNegativeDouble(3, 0, 2)) return 1;
		if (checkTableNonNegativeInteger(3, 2, 4)) return 1;
		if (checkNoiseParams()) return 1;
		return 0;
		};

	// set backgrounds
	auto setWhiteBg = [&](int ti, int i, int j) {
		for (i; i < j; ++i) {
			tableParams[ti]->item(i, 1)->setBackground(Qt::white);
		}
		};
	setWhiteBg(0, 0, 1);
	setWhiteBg(1, 0, 2);
	setWhiteBg(2, 1, 4);
	setWhiteBg(3, 0, 4);
	setWhiteBg(3, 6, tableParams[3]->rowCount());

	// block signals
	for (int i = 0; i < tableN; ++i) {
		tableParams[i]->blockSignals(1);
	}

	bool result = checkParams();

	// unblock signals
	for (int i = 0; i < tableN; ++i) {
		tableParams[i]->blockSignals(0);
	}

	if (result) return;

	// pop-up window with task
	if (task->newTask()) {
		resultLabel->hide();
		resultTable->hide();
		plotErrorInfo->hide();
		plotErrorSelector->hide();
		plotSignalInfo->hide();
		plotSignalSelector->hide();
		plotIterationInfo->hide();
		plotIterationSelector->hide();
		copyPlotToClipboard->hide();
		showTableButton->hide();
		if (plot) {
			plotLayout->removeWidget(plot);
			plot->setParent(0);
		}
		if (dataTable) {
			delete dataTable;
			dataTable = 0;
		}
		taskWidget->show();
		this->lockTables();
		return;
	}
	this->unlockTables();
	taskWidget->hide();
	plotSignalInfo->show();
	plotSignalSelector->show();
	copyPlotToClipboard->show();
	showTableButton->show();
	if (plot) {
		plotLayout->removeWidget(plot);
		plot->setParent(0);
	}

	// get params from input
	std::string bits = tableParams[1]->item(0, 1)->text().toStdString();
	int chunksize = tableParams[1]->item(1, 1)->text().toInt();
	bool modified = modifiedBox->currentIndex();
	conversionMethod signal_method = (conversionMethod)signalMethodBox->currentIndex();
	double signal_dt = tableParams[2]->item(1, 1)->text().toDouble();
	double signal_A = tableParams[2]->item(2, 1)->text().toDouble();
	int signal_DotsPerBit = tableParams[2]->item(3, 1)->text().toInt();
	bool signal_polarity = signalPolarBox->currentIndex();
	double noise_t = tableParams[3]->item(0, 1)->text().toDouble();
	double noise_dt = tableParams[3]->item(1, 1)->text().toDouble();
	int noise_nu = tableParams[3]->item(2, 1)->text().toInt();
	int noise_dnu = tableParams[3]->item(3, 1)->text().toInt();
	noiseForm noise_form = noiseForm::RECTANGULAR;
	bool noise_polarity = noisePolarBox->currentIndex();
	std::vector<double> noise_params;
	for (int i = 5; i < tableParams[3]->rowCount(); ++i) {
		noise_params.push_back(tableParams[3]->item(i, 1)->text().toDouble());
	}
	int iterations = tableParams[0]->item(0, 1)->text().toInt();

	// start experiments
	handler = HammingCodeHandler(
		bits, chunksize, modified,
		signal_method, signal_dt, signal_A,
		signal_DotsPerBit, signal_polarity,
		noise_t, noise_dt, noise_nu, noise_dnu,
		noise_form, noise_polarity, noise_params,
		iterations);

	handler.generate();

	delete dataTable;
	dataTable = new DataTable();

	// Set trust level
	resultLabel->show();
	resultTable->show();
	encodedItem->setText(QString::fromStdString(handler.coded));
	trustIntervalMinimumItem->setText(QString::number(handler.min, 'g', 2));
	trustIntervalMaximumItem->setText(QString::number(handler.max, 'g', 2));
	trustIntervalLevelItem->setText(QString::number(handler.trustlevel, 'g', 2));

	// Reset selectors
	plotErrorSelector->setCurrentIndex(0);
	plotSignalSelector->setCurrentIndex(0);
	plotIterationSelector->setValue(1);
	plotIterationSelector->setMaximum(iterations);

	// Add plot
	if (plot == nullptr) {
		plot = new QwtPlot;
		plot->setAutoReplot(true);
		plot->setAxisTitle(QwtPlot::xBottom, "t");
		plot->setAxisTitle(QwtPlot::yLeft, "A");

		auto grid = new QwtPlotGrid();
		grid->setMajorPen(QPen(Qt::gray, 1));
		grid->setMinorPen(QPen(Qt::lightGray, 1));
		grid->enableXMin(true);
		grid->enableYMin(true);
		grid->attach(plot);

		auto magnifier = new QwtPlotMagnifier(plot->canvas());
		magnifier->setMouseButton(Qt::MiddleButton);
		magnifier->setWheelFactor(1.1); // invert controls
		auto panner = new QwtPlotPanner(plot->canvas());
		panner->setMouseButton(Qt::LeftButton);

		// Use picker for displaying mouse position
		auto picker = new QwtPlotPicker(
			QwtPlot::xBottom, QwtPlot::yLeft,
			QwtPlotPicker::CrossRubberBand,
			QwtPicker::ActiveOnly,
			plot->canvas());
		picker->setRubberBandPen(QColor(Qt::red));
		picker->setTrackerPen(QColor(Qt::black));
		picker->setStateMachine(new QwtPickerDragPointMachine());
		{
			QVector<QwtEventPattern::MousePattern> pattern;
			pattern.push_back(QwtEventPattern::MousePattern(Qt::RightButton));
			picker->setMousePattern(pattern);
		}

		// Curve
		curve = new QwtPlotCurve;
		curve->setPen(Qt::blue, 2);
		curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

		QwtSymbol* symbol = new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::blue), QPen(Qt::blue, 0), QSize(8, 8));
		curve->setSymbol(symbol);
		curve->attach(plot);

		// Force update
		plotChanged();
	}

	plotLayout->addWidget(plot);
}
