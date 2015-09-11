
#ifndef _STATUSBAR_HANDLER_H
#define _STATUSBAR_HANDLER_H
#include <QtWidgets/QWidget>
#include <QtWidgets/QStatusBar>
#include <QString>

class tbStatusBar_Handler : QWidget
{
    Q_OBJECT

public:
    tbStatusBar_Handler(QStatusBar *statusbar);

    void update_status_bar(QString string, unsigned int how_long = 10000);

private:
    QStatusBar *m_statusbar;

};



#endif
