
#ifndef _TBLOCKER_H
#define _TBLOCKER_H

#include <QtWidgets/QMainWindow>
#include <QStandardPaths>
#include <QDir>
#include <QString>
#include <QDateTime>

#include "textedit_handler.h"
#include "statusbar_handler.h"
#include "keyDialog.h"
#include "calendarDialog.h"

extern "C"
{
    #include "blowfish.h"
}

typedef struct _diary_entry_t
{
    QByteArray start_tag;
    QByteArray datetime;
    QByteArray entry_text;
    QByteArray end_tag;
    _diary_entry_t *next;

} diary_entry_t;

typedef struct _diary_header_t
{
    QByteArray header;
} diary_header_t;

class tbLocker : public QObject
{
    Q_OBJECT

public:
    tbLocker(QMainWindow *parent, tbTextEdit_Handler *_textedit,  tbStatusBar_Handler *_statusbar);
    ~tbLocker();

public slots:
    void new_entry(void);
    void open_entry(void);
    void save_entry(void);
    void about(void);

private:
    QMainWindow *m_mainwindow;

    static const QString DIARY_FILE_NAME;
    static const QString DIARY_FILE_HEADER;
    static const QString DIARY_ENTRY_START_TAG;
    static const QString DIARY_ENTRY_END_TAG;
    static const QString DATETIME_SPACER_STRING;
    static const int DATETIME_STRING_LENGTH;
    static const int DATETIME_SPACER_LENGTH;

    static const int DIARY_BLOCK_SIZE_BYTES;
    static const int DIARY_HEADER_BLOCKS;
    static const int DIARY_DATETIME_BLOCKS;
    static const int DIARY_START_TAG_BLOCKS;
    static const int DIARY_END_TAG_BLOCKS;

    QString m_homepath;
    QString m_diary_filename;

    //statusBar object
    tbStatusBar_Handler *m_statusbar;
    void write_status(QString str);

    //the textEdit object
    tbTextEdit_Handler *m_textedit;

    //file header
    diary_header_t* m_header;
    //list of diary entries
    QList<diary_entry_t> m_diary_entry_list;
    void add_entry_to_list(diary_entry_t * list_entry);

    bool m_diary_file_read;
    bool read_diary_file(void);
    void save_diary_file(void);

    //The encrypted diary file
    QFile* m_diary_file;
    //Whether the file exists or not (detected at startup)
    bool m_file_exists;

    //Whether we've asked the user for the key and gotten one successfully
    bool m_have_key;
    //The key string, if any
    QString m_file_key;

    bool entry_exists(QDateTime dt, diary_entry_t **d_entry);

    //create new diary file if necessary (puts header at top of file)
    bool create_new_diary_file(void);

    //get key from user (keyDialog)
    bool get_key(void);
    QString get_datestring(void);

    bool reenter_key_mbox(int retries_left);
    //byte array representation of key
    QByteArray m_key_byte_array;
    //char* representation (QByteArray.data())
    char *m_key_array;
    
    //The blowfish context 
    blf_ctx m_c;
    //methods to encrypt a given string, or decrypt m_byte_buffer
    bool encrypt_buffer(QByteArray* arr);
    bool decrypt_buffer(QByteArray* arr);

    bool verify_diary_header(QByteArray *arr);
    void application_close_msg(void);

    //methods for debug
    void dump_entries(void);
    void test_encryption(void);
 
    //cant be copied or assigned
    tbLocker(const tbLocker&);
    tbLocker& operator = (const tbLocker&);
};

#endif


/****************************************************
PDL file entry format

<start/>
datetime
...bunch of text here
<///end>











***************************************************/
