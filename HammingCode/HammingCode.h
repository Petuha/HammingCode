#pragma once

#include <memory>
#include <QtWidgets/QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QGuiApplication>
#include <QScreen>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
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
	void resizeToContentAndCenter();
	void setTrustLevel(double min, double max, double lvl);
private:
	int i;
	QTableWidget* table;
};

struct CurvePair
{
public:
	CurvePair(
		std::unique_ptr<QwtPlotCurve> s = std::unique_ptr<QwtPlotCurve>(nullptr), 
		std::unique_ptr<QwtPlotCurve> r = std::unique_ptr<QwtPlotCurve>(nullptr)) 
		: sent(std::move(s)), received(std::move(r)) {}

	std::unique_ptr<QwtPlotCurve> sent;
	std::unique_ptr<QwtPlotCurve> received;
};

struct CurveSet 
{
public:
	CurvePair maxError;
	CurvePair minError;
	CurvePair medianError;
	CurvePair maxCorrectedError;
	std::vector<CurvePair> iterations;
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
	
	QwtPlot* plot;
	CurveSet curves;

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
