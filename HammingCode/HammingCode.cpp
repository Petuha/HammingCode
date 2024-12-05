#include "HammingCode.h"

#include "../SignalGenerator/SignalGenerator.h"
#include "../NoiseGenerator/NoiseGenerator.h"
#include <vector>
HammingCode::HammingCode(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    QLineSeries* series = new QLineSeries;
    series->setName("xyz");

    std::vector<std::pair<double, double>> data = generateSignalFromBits(0);
    std::vector<std::pair<double, double>> data1 = generateNoise(data);
    for (auto& x : data) series->append(x.first, x.second);

    QChart* chart = new QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    double r = 3, td = 0.05, h = 6;
    chart->axes(Qt::Vertical).first()->setRange(-2 * r, h + 2 * r);
    chart->axes(Qt::Horizontal).first()->setRange(-3 * r, 3 * r);
    chart->setVisible(1);
    QChartView* chartview = new QChartView(chart);
    chartview->setRenderHint(QPainter::Antialiasing);
    chartview->setVisible(1);

    setCentralWidget(chartview);
}

HammingCode::~HammingCode()
{}
