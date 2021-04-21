#include "JA_Projekt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    JA_Projekt w;
    w.show();
    return a.exec();
}
