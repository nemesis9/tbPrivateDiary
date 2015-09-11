#include "textedit_handler.h"
#include <QDateTime>
#include <iostream>
#include <QDebug>
#include <QtWidgets/QFontDialog>


tbTextEdit_Handler::tbTextEdit_Handler(QPlainTextEdit *textEdit) : 
    m_textEdit(textEdit), 
    m_cursor_absolute_start_position(0), 
    m_cursor_current_position(0), 
    m_opening_entry(false),
    m_new_entry(false)
{
    m_cursor = m_textEdit->textCursor();
    putDateTime();

    QObject::connect(m_textEdit, SIGNAL(textChanged()),
                     this, SLOT(text_changed()));

    QObject::connect(m_textEdit, SIGNAL(cursorPositionChanged()),
                     this, SLOT(cursor_changed()));

}


void 
tbTextEdit_Handler::set_entry_text(const QString& str)
{
    m_opening_entry = true;
    m_textEdit->setPlainText(str);    
}

void
tbTextEdit_Handler::new_entry(void)
{
    m_new_entry = true;
    m_textEdit->clear();
    m_cursor.setPosition(0);
    m_cursor = m_textEdit->textCursor();
    m_cursor_absolute_start_position = 0;
    putDateTime();
}

void
tbTextEdit_Handler::write_status(QString string)
{

    if (tbChild::statusbar)
    {
        tbChild::statusbar->update_status_bar(string);
    }
}


QString
tbTextEdit_Handler::get_text(void)
{
    return m_textEdit->toPlainText();
}

void
tbTextEdit_Handler::font_selection(void)
{
    bool ok;
    m_font = QFontDialog::getFont(&ok, m_font, this);
    m_textEdit->setFont(m_font);
}

void
tbTextEdit_Handler::cursor_changed()
{
    m_cursor = m_textEdit->textCursor();
    if (m_cursor.position() < m_cursor_absolute_start_position)
    {
        m_cursor.setPosition(m_cursor_absolute_start_position);
        m_textEdit->setTextCursor(m_cursor);
    }
}

void
tbTextEdit_Handler::text_changed(void)
{
    //if we are in opening entry mode, just reset the flag and return
    if (true == m_opening_entry)
    {
        m_opening_entry = false;
        return;
    }

    //ditto for new entry mode
    if (true == m_new_entry)
    {
        m_new_entry = false;
        return;
    }

    m_cursor = m_textEdit->textCursor();
    if (m_cursor.position() < m_cursor_absolute_start_position)
    {
        do {
            m_cursor.insertText(" ");
            m_textEdit->setTextCursor(m_cursor);
        } while (m_textEdit->textCursor().position() < m_cursor_absolute_start_position);
    }    
}


void 
tbTextEdit_Handler::set_statusbar(tbStatusBar_Handler *_statusbar)
{
    tbChild::set_statusbar(_statusbar);
}


int
tbTextEdit_Handler::putDateTime(void)
{
    QDateTime datetime = QDateTime::currentDateTime();
    QString dt_string = datetime.toString("ddd MMM dd hh:mm:ss yyyy");
    dt_string.append(" : ");
    
    m_cursor.insertText(dt_string);
    if (m_cursor_absolute_start_position == 0)
    {
        m_cursor_absolute_start_position = m_cursor_current_position = m_cursor.position();
        m_textEdit->setTextCursor(m_cursor);
    }
   
    return 0;
}

