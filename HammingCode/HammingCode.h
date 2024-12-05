#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtCharts>
#include "ui_HammingCode.h"

class HammingCode : public QMainWindow
{
    Q_OBJECT

public:
    HammingCode(QWidget *parent = nullptr);
    ~HammingCode();

private:
    Ui::HammingCodeClass ui;
};
