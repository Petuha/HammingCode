#include "HammingCode.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HammingCode w;
    w.show();
    return a.exec();
}
