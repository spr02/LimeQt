#include "mainwindow.h"
#include <QApplication>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_grid.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show(); //no need to show as we simply make a new qwt plot inside constructor

    return a.exec();
}
