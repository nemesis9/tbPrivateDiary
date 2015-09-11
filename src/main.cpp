#include "mainwindow.h"
#include <QtWidgets/QApplication>

static QApplication *a;

void
app_quit(void)
{
    a->quit();
}

int main(int argc, char *argv[])
{
    QApplication *a = new QApplication(argc, argv);
    MainWindow w;
    w.show();

    return a->exec();
}
