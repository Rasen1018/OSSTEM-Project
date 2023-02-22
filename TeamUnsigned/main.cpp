#include <QApplication>

#include "xraydb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    XrayDB d;
    d.show();

    return a.exec();
}
