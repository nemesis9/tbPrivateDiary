
#ifndef _TEXTEDIT_HANDLER_H
#define _TEXTEDIT_HANDLER_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPlainTextEdit>
#include "tbChild.h"

class tbTextEdit_Handler : QWidget, public tbChild
{
    Q_OBJECT

public:
    tbTextEdit_Handler(QPlainTextEdit *textEdit);
    void set_statusbar(tbStatusBar_Handler *_statusbar);
    QString get_text(void);

public slots:
    void text_changed(void);
    void cursor_changed();
    void font_selection(void);
    void set_entry_text(const QString& str);
    void new_entry(void);
  
private:
//Attributes
  // - widgets
    QPlainTextEdit *m_textEdit;
    QTextCursor m_cursor;
    QFont m_font;

  // - cursor attribs
    int m_cursor_absolute_start_position;
    int m_cursor_current_position; 
    bool m_opening_entry;
    bool m_new_entry;
  

//Methods
    int putDateTime(void);
    void write_status(QString string);



};

#endif

