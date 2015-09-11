#ifndef _KEYDIALOG_H
#define _KEYDIALOG_H

#include <QtWidgets/QDialog>

#include "ui_keyDialog.h"

class keyDialog : public QDialog, public Ui::keyDialog
{
    Q_OBJECT

public:
    keyDialog(QWidget *parent = 0);

    bool get_text(QString *linetext);

private:
    Ui::keyDialog *ui;
    bool m_linetext_valid;
    QString linetext;
    void get_key(void);

private slots:
    void on_keyDialog_lineEdit_textChanged();

};
#endif
