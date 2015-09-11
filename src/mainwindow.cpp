#include <QtWidgets/QWidget>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

// Setup the UI  
    ui->setupUi(this);

// Set the fleur image on the left and right labels
    leftLabel = ui->leftLabel;
    rightLabel = ui->rightLabel;

    QImage *image = new QImage(":/fleur.png");
    if (image != NULL)
    {
        QPixmap *pixmap = new QPixmap();
        pixmap->convertFromImage(*image);
        leftLabel->setPixmap(*pixmap); 
        rightLabel->setPixmap(*pixmap); 
    }

//Instantiate statusbar handler
    statusbar_handler = new tbStatusBar_Handler(ui->statusBar); 
    statusbar_handler->update_status_bar(QString("Welcome to your private diary"));

//Instantiate textedit handler
    textedit_handler = new tbTextEdit_Handler(ui->textEdit);    
    textedit_handler->set_statusbar(statusbar_handler);

//instantiate locker
    locker = new tbLocker(this, textedit_handler, statusbar_handler);

//assign slots for menu items
    QObject::connect(ui->actionFont_Diary_Entry, SIGNAL(triggered()),
                     (QObject*)textedit_handler, SLOT(font_selection()));


    QObject::connect(ui->actionNew_Diary_Entry, SIGNAL(triggered()),
                     (QObject*)locker, SLOT(new_entry()));
 
    QObject::connect(ui->actionOpen_Diary_Entry, SIGNAL(triggered()),
                     (QObject*)locker, SLOT(open_entry()));

    QObject::connect(ui->actionSave_Diary_Entry, SIGNAL(triggered()),
                     (QObject*)locker, SLOT(save_entry()));

    QObject::connect(ui->actionAbout, SIGNAL(triggered()),
                     (QObject*)locker, SLOT(about()));


}

MainWindow::~MainWindow()
{
    delete ui;
}
