
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include "calendarDialog.h"
#include "ui_calendarDialog.h"

calendarDialog::calendarDialog(QWidget *parent)
  : QDialog(parent)
{

    setupUi(this);
    cal_widget = ui->calendarWidget;
    //calendarDialog_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    
    //QRegExp regExp("[A-Za-z0-9]{8,56}");
    //keyDialog_lineEdit->setValidator(new QRegExpValidator(regExp, this));

    //connect(keyDialog_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    //connect(keyDialog_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
}

//void keyDialog::on_keyDialog_lineEdit_textChanged()
//{
//    keyDialog_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
//            keyDialog_lineEdit->hasAcceptableInput());
//}


//bool 
//keyDialog::get_text(QString *linetext)
//{
//    QString strout = keyDialog_lineEdit->text();
//    
//    if (keyDialog_lineEdit->hasAcceptableInput())
//    {
//        *linetext = strout;
//        return true;
//    }
//    else
//    {
//        return false;
//    }

//}
