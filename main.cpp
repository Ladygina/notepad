#include "mainwindow.h"
#include"highlighter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    w.setWindowIcon(QIcon(QObject::tr(":toolBar/images/main.jpg")));
    return a.exec();
}
