#include "AoC_2023.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AoC_2023 w(argv[0]);
    w.show();
    return a.exec();
}
