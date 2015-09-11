

#include "statusbar_handler.h"


tbStatusBar_Handler::tbStatusBar_Handler(QStatusBar *statusbar) :  m_statusbar(statusbar)
{
} 


void
tbStatusBar_Handler::update_status_bar(QString string, unsigned int how_long) 
{
    m_statusbar->showMessage(string, how_long);
}
