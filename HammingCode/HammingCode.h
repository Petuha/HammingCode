#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtCharts>
#include <QtWidgets/Qwidget>
#include <QtWidgets/QComboBox>
#include "ui_HammingCode.h"

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
        if (this->hasFocus()) {
            QComboBox::wheelEvent(e);
        }
    }
};

class HammingCode : public QMainWindow
{
    Q_OBJECT

public:
    HammingCode(QWidget *parent = nullptr);
    ~HammingCode();
protected slots:
    void calculate_clicked();
    void noiseChanged(int index);
    void plotChanged(int index);
private:
    Ui::HammingCodeClass ui;

    enum { plotN = 4, plotM = 2 };
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

    QTableWidget* tableParams[3];
    QLabel* label[3];

    FocusWhellComboBox* noiseTypeBox;
    FocusWhellComboBox* noisePolarBox;

    FocusWhellComboBox* signalMethodBox;
    FocusWhellComboBox* signalPolarBox;
};
