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

class HammingCode : public QMainWindow
{
	Q_OBJECT

public:
	HammingCode(QWidget* parent = nullptr);
	~HammingCode();
protected slots:
	void calculate_clicked();
	void noiseChanged(int index);
	void plotChanged(int index);
private:
	Ui::HammingCodeClass ui;

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
