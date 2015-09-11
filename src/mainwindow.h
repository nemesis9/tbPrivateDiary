#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QStatusBar>
#include "statusbar_handler.h"
#include "textedit_handler.h"
#include "tbLocker.h"

class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    tbStatusBar_Handler status_bar(void);

private:
    Ui::MainWindow *ui;

    QLabel *leftLabel;
    QLabel *rightLabel;

    tbTextEdit_Handler *textedit_handler;
    tbStatusBar_Handler *statusbar_handler;

    tbLocker *locker;
};

#endif // MAINWINDOW_H
