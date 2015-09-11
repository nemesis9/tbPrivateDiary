
#ifndef _TBCHILD_H
#define _TBCHILD_H

#include "statusbar_handler.h"

class tbChild
{
public:
    void set_statusbar(tbStatusBar_Handler *status_bar);


protected:
    tbStatusBar_Handler *statusbar;
    
};





#endif
