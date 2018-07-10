#include "zmainui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ZMainUI w;
    w.show();

    return a.exec();
}
