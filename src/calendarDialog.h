#ifndef _CALENDARDIALOG_H
#define _CALENDARDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_calendarDialog.h"

class calendarDialog : public QDialog, public Ui::calendarDialog
{
    Q_OBJECT

public:
    calendarDialog(QWidget *parent = 0);

    //bool get_text(QString *linetext);

private:
    Ui::calendarDialog *ui;
    QCalendarWidget* cal_widget; 

private slots:

};
#endif
