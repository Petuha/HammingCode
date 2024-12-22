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

	this->setMinimumSize(1067, 600);

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


	// Labels
	label[0] = new QLabel("Код Хэмминга", ui.centralWidget);
	label[1] = new QLabel("Сигнал", ui.centralWidget);
	label[2] = new QLabel("Помеха", ui.centralWidget);

	// Table params
	tableParams[0] = new QTableWidget(2, 2, ui.centralWidget);
	tableParams[1] = new QTableWidget(5, 2, ui.centralWidget);
	tableParams[2] = new QTableWidget(7, 2, ui.centralWidget);
	for (int param = 0; param < 3; ++param) {
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
		int tableW = tableParams[0]->horizontalHeader()->defaultSectionSize();
		int tableH = tableParams[0]->verticalHeader()->defaultSectionSize();
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
		for (int i = 0; i < 3; ++i) {
			setLabel(i);
			y += label[i]->geometry().height();
			setTableParams(i);
			y += tableParams[i]->geometry().height();
		}
		};
	setTableParamsLabel();

	// Set table values

	// Hamming values
	tableParams[0]->item(0, 0)->setText("Последовательность");
	tableParams[0]->item(1, 0)->setText("Размер блока");

	// Signal values
	tableParams[1]->item(0, 0)->setText("Метод преобразования");
	tableParams[1]->item(1, 0)->setText("Шаг дискретизации");
	tableParams[1]->item(2, 0)->setText("Амплитуда");
	tableParams[1]->item(3, 0)->setText("Длительность битового интервала");
	tableParams[1]->item(4, 0)->setText("Полярность");

	// Noise values
	{
		QSize cellSize;
		cellSize.setWidth(tableParams[0]->horizontalHeader()->defaultSectionSize());
		cellSize.setHeight(tableParams[0]->verticalHeader()->defaultSectionSize());
		tableParams[2]->setIconSize(cellSize);
	}
	tableParams[2]->item(0, 0)->setIcon(htmlText("<math>t</math>"));
	tableParams[2]->item(1, 0)->setIcon(htmlText("<math>&Delta;t</math>"));
	tableParams[2]->item(2, 0)->setIcon(htmlText("<math>&nu;</math>"));
	tableParams[2]->item(3, 0)->setIcon(htmlText("<math>&Delta;&nu;</math>"));
	tableParams[2]->item(4, 0)->setIcon(htmlText("<math>Форма</math>"));
	tableParams[2]->item(5, 0)->setIcon(htmlText("<math>Полярность</math>"));
	tableParams[2]->item(6, 0)->setIcon(htmlText("<math>a</math>"));

	// Signal Qcombos
	signalMethodBox = new FocusWhellComboBox(ui.centralWidget);
	signalMethodBox->addItem("NRZ");
	signalMethodBox->addItem("Манчестерский");
	signalMethodBox->addItem("RZ");
	signalMethodBox->addItem("AMI");
	tableParams[1]->setCellWidget(0, 1, signalMethodBox);

	signalPolarBox = new FocusWhellComboBox(ui.centralWidget);
	signalPolarBox->addItem("Прямая");
	signalPolarBox->addItem("Инвертированная");
	tableParams[1]->setCellWidget(4, 1, signalPolarBox);

	// Noise Qcombos
	noiseTypeBox = new FocusWhellComboBox(ui.centralWidget);
	noiseTypeBox->setIconSize(tableParams[2]->iconSize());
	noiseTypeBox->addItem(QIcon(htmlText("<math>a</math>")), "");
	noiseTypeBox->addItem(QIcon(htmlText("<math>a sin(bx)</math>")), "");
	noiseTypeBox->addItem(QIcon(htmlText("<math>ax<sup>2</sup></math>")), "");
	tableParams[2]->setCellWidget(4, 1, noiseTypeBox);

	noisePolarBox = new FocusWhellComboBox(ui.centralWidget);
	noisePolarBox->addItem("Однополярный");
	noisePolarBox->addItem("Биполярный");
	tableParams[2]->setCellWidget(5, 1, noisePolarBox);

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
	plotSignalSelector->addItem("Отправляеый");
	plotSignalSelector->addItem("Принимаемый");
	plotSignalSelector->hide();

	plotInfoWidget = new QWidget(ui.centralWidget);
	setX(plotInfoWidget, ui.calculate->x() + ui.calculate->width() + 10);
	setY(plotInfoWidget, ui.calculate->y());
	setWidth(plotInfoWidget, 450);
	setHeight(plotInfoWidget, plotErrorSelector->height());
	//plotInfoWidget->setStyleSheet("background-color: lightblue;");

	// Plot Info Area
	plotInfoLayout = new QHBoxLayout(plotInfoWidget);
	plotInfoLayout->setContentsMargins(0, 0, 0, 0);

	plotInfoLayout->addWidget(plotErrorInfo);
	plotInfoLayout->addWidget(plotErrorSelector);
	plotInfoLayout->addWidget(plotSignalInfo);
	plotInfoLayout->addWidget(plotSignalSelector);


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

	connect(noiseTypeBox, &FocusWhellComboBox::currentIndexChanged, this, &HammingCode::noiseChanged);
	connect(plotErrorSelector, &FocusWhellComboBox::currentIndexChanged, this, &HammingCode::plotChanged);
	connect(plotSignalSelector, &FocusWhellComboBox::currentIndexChanged, this, &HammingCode::plotChanged);

	this->setStatusBar(0);
}


HammingCode::~HammingCode()
{
}

void HammingCode::noiseChanged(int index)
{
	static int pindex = -1;
	if (index == 1) {
		tableParams[2]->setRowCount(tableParams[2]->rowCount() + 1);
		tableParams[2]->setItem(tableParams[2]->rowCount() - 1, 0, new QTableWidgetItem());
		tableParams[2]->setItem(tableParams[2]->rowCount() - 1, 1, new QTableWidgetItem());
		tableParams[2]->item(tableParams[2]->rowCount() - 1, 0)->setFlags(Qt::ItemIsEnabled);
		tableParams[2]->item(tableParams[2]->rowCount() - 1, 0)->setIcon(htmlText("<math>b</math>"));
		setHeight(tableParams[2],
			tableParams[2]->height() + tableParams[2]->verticalHeader()->defaultSectionSize());
	}
	else {
		if (pindex == 1) {
			delete tableParams[2]->item(tableParams[2]->rowCount() - 1, 0);
			delete tableParams[2]->item(tableParams[2]->rowCount() - 1, 1);
			setHeight(tableParams[2],
				tableParams[2]->height() - tableParams[2]->verticalHeader()->defaultSectionSize());
			tableParams[2]->setRowCount(tableParams[2]->rowCount() - 1);
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

void HammingCode::calculate_clicked()
{
	//ui.statusBar->showMessage(QString("some text"));
	plotErrorInfo->show();
	plotErrorSelector->show();
	plotErrorSelector->setCurrentIndex(0);
	plotSignalInfo->show();
	plotSignalSelector->show();
	plotSignalSelector->setCurrentIndex(0);
	if (pchartview) {
		plotLayout->removeWidget(pchartview);
		pchartview->setParent(0);
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

	newPlot(0, 0, generateSignalFromBits("", conversionMethod::AMI, 0, 0, 0, 0));
	std::vector<Dot> d = { {0, 0}, {0, 1}, {1, 1} };
	newPlot(1, 1, d);
	for (int i = 0; i < plotN; ++i) {
		for (int j = 0; j < plotM; ++j) {
			if (i == 0 && j == 0 || i == 1 && j == 1) continue;
			newPlot(i, j, {});
		}
	}


	plotLayout->addWidget(chartview[0][0]);
	pchartview = chartview[0][0];
}
