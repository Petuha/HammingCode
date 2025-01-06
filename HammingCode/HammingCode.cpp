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

	this->setMinimumSize(1280, 720);

	// Plots
	for (int i = 0; i < plotN; ++i) {
		for (int j = 0; j < plotM; ++j) {
			series[i][j] = 0;
			chart[i][j] = 0;
			chartview[i][j] = 0;
		}
	}

	int windowH = ui.centralWidget->parentWidget()->geometry().height();
	int windowW = ui.centralWidget->parentWidget()->geometry().width();


	// Table labels
	label[0] = new QLabel("Эксперимент", ui.centralWidget);
	label[1] = new QLabel("Код Хэмминга", ui.centralWidget);
	label[2] = new QLabel("Сигнал", ui.centralWidget);
	label[3] = new QLabel("Помеха", ui.centralWidget);

	// Table params
	tableParams[1] = new QTableWidget(3, 2, ui.centralWidget);
	tableParams[2] = new QTableWidget(5, 2, ui.centralWidget);
	tableParams[3] = new QTableWidget(8, 2, ui.centralWidget);
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
	tableParams[2]->item(3, 0)->setText("Битовый интервал");
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
	tableParams[3]->item(4, 0)->setIcon(htmlText("<math>Форма</math>"));
	tableParams[3]->item(5, 0)->setIcon(htmlText("<math>Полярность</math>"));
	tableParams[3]->item(6, 0)->setIcon(htmlText("<math>a</math>"));
	tableParams[3]->item(7, 0)->setIcon(htmlText("<math>&Delta;a</math>"));

	// Experiment values
	tableParams[0]->item(0, 0)->setText("Число итераций");

	// Hamming QCombos
	modifiedBox = new FocusWhellComboBox(ui.centralWidget);
	modifiedBox->addItem("Да");
	modifiedBox->addItem("Нет");
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
	noiseTypeBox = new FocusWhellComboBox(ui.centralWidget);
	noiseTypeBox->setIconSize(tableParams[3]->iconSize());
	noiseTypeBox->addItem(QIcon(htmlText("<math>a</math>")), "");
	noiseTypeBox->addItem(QIcon(htmlText("<math>a sin(bx)</math>")), "");
	noiseTypeBox->addItem(QIcon(htmlText("<math>ax<sup>2</sup></math>")), "");
	tableParams[3]->setCellWidget(4, 1, noiseTypeBox);

	noisePolarBox = new FocusWhellComboBox(ui.centralWidget);
	noisePolarBox->addItem("Однополярный");
	noisePolarBox->addItem("Биполярный");
	tableParams[3]->setCellWidget(5, 1, noisePolarBox);

	// Calculate button
	setX(ui.calculate, tableParams[0]->width() + 30);
	setY(ui.calculate, tableParams[0]->y());

	// Plot Info Selectors
	plotErrorSelector = new FocusWhellComboBox(ui.centralWidget);
	plotErrorSelector->addItem("Наибольшая");
	plotErrorSelector->addItem("Наименьшая");
	plotErrorSelector->addItem("Медианная");
	plotErrorSelector->addItem("Наибольшая верно исправленная");
	plotErrorSelector->hide();

	plotErrorInfo = new QLabel(ui.centralWidget);
	plotErrorInfo->setText("Ошибка");
	plotErrorInfo->hide();

	plotSignalInfo = new QLabel(ui.centralWidget);
	plotSignalInfo->setText("Сигнал");
	plotSignalInfo->hide();

	plotSignalSelector = new FocusWhellComboBox(ui.centralWidget);
	plotSignalSelector->addItem("Отправляемый");
	plotSignalSelector->addItem("Принимаемый");
	plotSignalSelector->hide();

	copyPlotToClipboard = new QPushButton(ui.centralWidget);
	copyPlotToClipboard->setText("Копировать в буфер");
	copyPlotToClipboard->hide();

	showTableButton = new QPushButton(ui.centralWidget);
	showTableButton->setText("Показать таблицу");
	showTableButton->hide();

	plotInfoWidget = new QWidget(ui.centralWidget);
	setX(plotInfoWidget, ui.calculate->x() + ui.calculate->width() + 10);
	setY(plotInfoWidget, ui.calculate->y());
	setWidth(plotInfoWidget, 700);
	setHeight(plotInfoWidget, plotErrorSelector->height());
	//plotInfoWidget->setStyleSheet("background-color: lightblue;");

	// Plot Info Area
	plotInfoLayout = new QHBoxLayout(plotInfoWidget);
	plotInfoLayout->setContentsMargins(0, 0, 0, 0);

	plotInfoLayout->addWidget(plotErrorInfo);
	plotInfoLayout->addWidget(plotErrorSelector);
	plotInfoLayout->addWidget(plotSignalInfo);
	plotInfoLayout->addWidget(plotSignalSelector);
	plotInfoLayout->addWidget(copyPlotToClipboard);
	plotInfoLayout->addWidget(showTableButton);


	// Plot Layout
	plotWidget = new QWidget(ui.centralWidget);
	plotWidget->setGeometry(
		plotInfoWidget->x(),
		plotInfoWidget->y() + plotInfoWidget->height(),
		windowW - plotInfoWidget->x(),
		windowH - (plotInfoWidget->y() + plotInfoWidget->height()));
	//plotWidget->setStyleSheet("background-color: lightblue;");
	plotLayout = new QHBoxLayout(plotWidget);
	plotLayout->setContentsMargins(0, 0, 0, 0);

	// connects
	connect(noiseTypeBox, &FocusWhellComboBox::currentIndexChanged, this, &HammingCode::noiseChanged);
	connect(plotErrorSelector, &FocusWhellComboBox::currentIndexChanged, this, &HammingCode::plotChanged);
	connect(plotSignalSelector, &FocusWhellComboBox::currentIndexChanged, this, &HammingCode::plotChanged);
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
	setWidth(taskWidget, 500);
	setHeight(taskWidget, 500);
	setX(taskWidget, plotWidget->x() + (plotWidget->width() - taskWidget->width()) / 2);
	setY(taskWidget, plotWidget->y() + (plotWidget->height() - taskWidget->height()) / 2);
	//taskWidget->setStyleSheet("background-color: lightblue;");
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
	setDeafaultTableParams(tableParams[3], 6, tableParams[3]->rowCount());


}


HammingCode::~HammingCode()
{
}

void HammingCode::showTableClicked()
{
	if (dataTable) {
		dataTable->hide();
		dataTable->show();
	}
}

void HammingCode::copyClicked()
{
	int maxWidth = 1920, maxHeight = 1080;
	QChartView* chartView = chartview[plotErrorSelector->currentIndex()][plotSignalSelector->currentIndex()];
	if (!chartView) return;
	int width = chartView->width();
	int height = chartView->height();
	if (1.0 * height * maxWidth / width <= maxHeight) {
		height = static_cast<int>(1.0 * height * maxWidth / width);
		width = maxWidth;
	}
	else {
		width = static_cast<int>(1.0 * width * maxHeight / height);
		height = maxHeight;
	}
	QImage image(width, height, QImage::Format_ARGB32);
	image.fill(Qt::transparent);
	QPainter painter(&image);
	chartView->render(&painter);
	painter.end();
	QClipboard* clipboard = QGuiApplication::clipboard();
	if (!clipboard) return;
	clipboard->setImage(image);
}

void HammingCode::noiseChanged(int index)
{
	static int pindex = -1;
	if (index == 1) {
		tableParams[3]->setRowCount(tableParams[3]->rowCount() + 2);

		tableParams[3]->setItem(tableParams[3]->rowCount() - 2, 0, new QTableWidgetItem());
		tableParams[3]->setItem(tableParams[3]->rowCount() - 2, 1, new QTableWidgetItem());
		tableParams[3]->setItem(tableParams[3]->rowCount() - 1, 0, new QTableWidgetItem());
		tableParams[3]->setItem(tableParams[3]->rowCount() - 1, 1, new QTableWidgetItem());

		tableParams[3]->item(tableParams[3]->rowCount() - 2, 0)->setFlags(Qt::ItemIsEnabled);
		tableParams[3]->item(tableParams[3]->rowCount() - 2, 0)->setIcon(htmlText("<math>b</math>"));
		tableParams[3]->item(tableParams[3]->rowCount() - 1, 0)->setFlags(Qt::ItemIsEnabled);
		tableParams[3]->item(tableParams[3]->rowCount() - 1, 0)->setIcon(htmlText("<math>&Delta;b</math>"));

		setHeight(tableParams[3],
			tableParams[3]->height() + 2 * tableParams[3]->verticalHeader()->defaultSectionSize());
	}
	else {
		if (pindex == 1) {
			delete tableParams[3]->item(tableParams[3]->rowCount() - 2, 0);
			delete tableParams[3]->item(tableParams[3]->rowCount() - 2, 1);
			delete tableParams[3]->item(tableParams[3]->rowCount() - 1, 0);
			delete tableParams[3]->item(tableParams[3]->rowCount() - 1, 1);

			setHeight(tableParams[3],
				tableParams[3]->height() - 2 * tableParams[3]->verticalHeader()->defaultSectionSize());

			tableParams[3]->setRowCount(tableParams[3]->rowCount() - 2);
		}
	}
	pindex = index;
}

void HammingCode::plotChanged(int index)
{
	//ui.statusBar->showMessage(QString::number(plotTypeSelector->currentIndex()) + " " + 
		//QString::number(plotSignalSelector->currentIndex()));

	if (pchartview) {
		plotLayout->removeWidget(pchartview);
		pchartview->setParent(0);
	}
	QChartView* ptr = chartview[plotErrorSelector->currentIndex()][plotSignalSelector->currentIndex()];
	if (!ptr) {
		//ui.statusBar->showMessage("Pizda");
		return;
	}
	plotLayout->addWidget(ptr);
	pchartview = ptr;
}

void HammingCode::itemChanged(QTableWidgetItem* item)
{
	item->setBackground(Qt::white);
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
			for (int i = 6, parity = 0; i < tableParams[3]->rowCount(); ++i, ++parity) {
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

		if (checkTablePositiveDouble(2, 1, 4)) return 1;
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
		plotErrorInfo->hide();
		plotErrorSelector->hide();
		plotErrorSelector->setCurrentIndex(0);
		plotSignalInfo->hide();
		plotSignalSelector->hide();
		plotSignalSelector->setCurrentIndex(0);
		copyPlotToClipboard->hide();
		showTableButton->hide();
		if (pchartview) {
			plotLayout->removeWidget(pchartview);
			pchartview->setParent(0);
			pchartview = 0;
		}
		if (dataTable) {
			delete dataTable;
			dataTable = 0;
		}
		task->show();
		return;
	}
	task->hide();
	plotErrorInfo->show();
	plotErrorSelector->show();
	plotErrorSelector->setCurrentIndex(0);
	plotSignalInfo->show();
	plotSignalSelector->show();
	plotSignalSelector->setCurrentIndex(0);
	copyPlotToClipboard->show();
	showTableButton->show();
	if (pchartview) {
		plotLayout->removeWidget(pchartview);
		pchartview->setParent(0);
		pchartview = 0;
	}
	for (int i = 0; i < plotN; ++i) {
		for (int j = 0; j < plotM; ++j) {
			delete series[i][j];
			delete chart[i][j];
			delete chartview[i][j];
			series[i][j] = 0;
			chart[i][j] = 0;
			chartview[i][j] = 0;
		}
	}

	// get params from input
	std::string bits = tableParams[1]->item(0, 1)->text().toStdString();
	int chunksize = tableParams[1]->item(1, 1)->text().toInt();
	bool modified = modifiedBox->currentIndex();
	conversionMethod signal_method = (conversionMethod)signalMethodBox->currentIndex();
	double signal_dt = tableParams[2]->item(1, 1)->text().toDouble();
	double signal_A = tableParams[2]->item(2, 1)->text().toDouble();
	double signal_bitDuration = tableParams[2]->item(3, 1)->text().toDouble();
	bool signal_polarity = signalPolarBox->currentIndex();
	double noise_t = tableParams[3]->item(0, 1)->text().toDouble();
	double noise_dt = tableParams[3]->item(1, 1)->text().toDouble();
	int noise_nu = tableParams[3]->item(2, 1)->text().toInt();
	int noise_dnu = tableParams[3]->item(3, 1)->text().toInt();
	noiseForm noise_form = (noiseForm)noiseTypeBox->currentIndex();
	bool noise_polarity = noisePolarBox->currentIndex();
	std::vector<double> noise_params;
	for (int i = 6; i < tableParams[3]->rowCount(); ++i) {
		noise_params.push_back(tableParams[3]->item(i, 1)->text().toDouble());
	}
	int iterations = tableParams[0]->item(0, 1)->text().toInt();

	// start experiments
	HammingCodeHandler handler(
		bits, chunksize, modified,
		signal_method, signal_dt, signal_A,
		signal_bitDuration, signal_polarity,
		noise_t, noise_dt, noise_nu, noise_dnu,
		noise_form, noise_polarity, noise_params,
		iterations);

	delete dataTable;
	dataTable = new DataTable(iterations, modified);
	//dataTable->show();

	std::vector<std::string> rowData;
	do {
		rowData = handler.next();
		// !!! add data to table
		dataTable->addRow(rowData);

	} while (rowData.size());
	// !!! add handler.trustlevel to table or to another place
	dataTable->setTrustLevel(handler.min, handler.max, handler.trustlevel);

	// add plots to view
	auto newPlot = [&](int i, int j, const std::vector<Dot>& data) {
		series[i][j] = new QLineSeries;
		double margin = 3, minY = 0, minX = 0, maxY = 0, maxX = 0;
		if (data.size()) {
			minX = maxX = data[0].x;
			minY = maxY = data[0].y;
		}
		for (auto& x : data) {
			series[i][j]->append(x.x, x.y);
			if (x.x < minX) minX = x.x;
			if (x.x > maxX) maxX = x.x;
			if (x.y < minY) minY = x.y;
			if (x.y > maxY) maxY = x.y;
		}

		chart[i][j] = new QChart;
		chart[i][j]->legend()->hide();
		chart[i][j]->addSeries(series[i][j]);
		chart[i][j]->createDefaultAxes();
		chart[i][j]->axes(Qt::Horizontal).first()->setRange(minX - margin, maxX + margin);
		chart[i][j]->axes(Qt::Vertical).first()->setRange(minY - margin, maxY + margin);

		chart[i][j]->layout()->setContentsMargins(0, 0, 0, 0);
		chart[i][j]->setBackgroundRoundness(0);

		chartview[i][j] = new QChartView(chart[i][j]);
		chartview[i][j]->setRenderHint(QPainter::Antialiasing);
		};
	for (int i = 0; i < plotN; ++i) {
		for (int j = 0; j < plotM; ++j) {
			newPlot(i, j, handler.plots[i][j]);
		}
	}
	plotLayout->addWidget(chartview[0][0]);
	pchartview = chartview[0][0];
}
