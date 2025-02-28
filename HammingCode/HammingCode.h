#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtCharts>
#include <QtWidgets/Qwidget>
#include <QtWidgets/QComboBox>
#include "ui_HammingCode.h"
#include "../CoderDecoder/CoderDecoder.h"

class FocusWhellComboBox : public QComboBox
{
	Q_OBJECT

public:
	explicit FocusWhellComboBox(QWidget* parent = nullptr)
		: QComboBox(parent)
	{
		this->setFocusPolicy(Qt::StrongFocus);
	}

	void wheelEvent(QWheelEvent* e) override
	{
		//if (this->hasFocus()) {
		//    QComboBox::wheelEvent(e);
		//}
	}
};

class HammingCode;

class TaskViewer : public QWidget
{
	Q_OBJECT

public:
	TaskViewer(QWidget* parent, HammingCode& mainClass);
	~TaskViewer();
	void show();
	void hide();
	bool newTask();
protected slots:
	void check();
	void answerChanged(QTableWidgetItem* item);
	void verdictChanged(int index);
private:
	HammingCode& mainClass;
	TaskManager manager;
	QLabel* taskLabel;
	QLabel* answerLabel;
	QLabel* verdictLabel;
	QTableWidget* table;
	QTableWidget* answer;
	FocusWhellComboBox* verdictBox;
	QPushButton* checkButton;
	bool hasTask = 0;
	int task_num = 0;
};

class DataTable : public QWidget
{
	Q_OBJECT

public:
	DataTable(int iterations, bool modified);
	void addRow(const std::vector<std::string>& data);
	void setTrustLevel(double min, double max, double lvl);
	~DataTable();
	void show();
	void hide();
private:
	int i;
	QTableWidget* table;
};

class HammingCode : public QMainWindow
{
	Q_OBJECT

public:
	HammingCode(QWidget* parent = nullptr);
	~HammingCode();
protected slots:
	void calculate_clicked();
	void showTableClicked();
	void copyClicked();
	void noiseChanged(int index);
	void plotChanged(int index);
	void itemChanged(QTableWidgetItem* item);
private:
	Ui::HammingCodeClass ui;

	void setTablesEnabled(bool flag);
	void lockTables();
	void unlockTables();

	enum { plotN = HammingCodeHandler::Plot::plotN, plotM = HammingCodeHandler::Plot::plotM };
	QLineSeries* series[plotN][plotM];
	QChart* chart[plotN][plotM];
	QChartView* chartview[plotN][plotM];
	QChartView* pchartview = 0;
	QWidget* plotWidget;
	QHBoxLayout* plotLayout;

	QWidget* plotInfoWidget;
	QHBoxLayout* plotInfoLayout;

	QLabel* plotErrorInfo;
	FocusWhellComboBox* plotErrorSelector;
	QLabel* plotSignalInfo;
	FocusWhellComboBox* plotSignalSelector;
	QPushButton* copyPlotToClipboard;
	QPushButton* showTableButton;

	DataTable* dataTable = 0;

	enum { tableN = 4 };
	QTableWidget* tableParams[tableN];
	QLabel* label[tableN];

	FocusWhellComboBox* modifiedBox;

	FocusWhellComboBox* noiseTypeBox;
	FocusWhellComboBox* noisePolarBox;

	FocusWhellComboBox* signalMethodBox;
	FocusWhellComboBox* signalPolarBox;

	friend class TaskViewer;
	QWidget* taskWidget;
	TaskViewer* task;
};
