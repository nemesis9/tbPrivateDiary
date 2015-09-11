#ifndef _SELECTENTRYDIALOG_H
#define _SELECTENTRYDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_selectEntryDialog.h"

class selectEntryDialog : public QDialog, public Ui::selectEntryDialog
{
    Q_OBJECT

public:
    selectEntryDialog(QWidget *parent = 0);

    void add_item(QString str);
    QString get_selection(void) { return selection; }

public slots:
    void accept(void);
    void reject(void);
    void on_itemPressed(QListWidgetItem *item);

private:
    Ui::selectEntryDialog *ui;
    int row;
    QString selection;

private slots:

};
#endif
