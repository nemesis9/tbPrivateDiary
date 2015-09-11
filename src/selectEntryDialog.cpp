
#include <QtWidgets/QWidget>
#include <QDebug>
#include <QtWidgets/QPushButton>
#include "selectEntryDialog.h"
#include "ui_selectEntryDialog.h"

selectEntryDialog::selectEntryDialog(QWidget *parent)
  : QDialog(parent),
    row(0)
{

    setupUi(this);
    selection.clear();
    
    connect(selectEntry_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(selectEntry_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(selectEntry_listWidget, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(on_itemPressed(QListWidgetItem *)));
}

void 
selectEntryDialog::add_item(QString str)
{
    selectEntry_listWidget->insertItem(row++, new QListWidgetItem(str, 0, 0)); 
}

void 
selectEntryDialog::accept(void)
{
    QDialog::accept();
}


void 
selectEntryDialog::reject(void)
{
    QDialog::reject();
    selection.clear();
}


void 
selectEntryDialog::on_itemPressed(QListWidgetItem *item)
{
    selection = item->text();
}


