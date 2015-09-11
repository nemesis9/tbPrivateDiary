#include "main.h"
#include "diary_version.h"
#include "tbLocker.h"
#include "selectEntryDialog.h"
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QString>
#include <iostream>



const QString tbLocker::DIARY_FILE_NAME = "private_diary.pdl";
const QString tbLocker::DIARY_FILE_HEADER = "tbPrivateDiary file version ";
const QString tbLocker::DIARY_ENTRY_START_TAG = "<entry/>";
const QString tbLocker::DIARY_ENTRY_END_TAG = "</entry>";
const QString tbLocker::DATETIME_SPACER_STRING = " : ";

const int tbLocker::DATETIME_STRING_LENGTH = 24;
const int tbLocker::DATETIME_SPACER_LENGTH = 3;

const int tbLocker::DIARY_BLOCK_SIZE_BYTES = 8;
const int tbLocker::DIARY_HEADER_BLOCKS = 4;
const int tbLocker::DIARY_DATETIME_BLOCKS = 4;
const int tbLocker::DIARY_START_TAG_BLOCKS = 1;  //unencrypted
const int tbLocker::DIARY_END_TAG_BLOCKS = 1;    //unencrypted

/**********************************************************************
 * tbLocker constructor
 * 
 * determines the home path
 * checks to see if a diary exists already, but does not open it
 *
 *********************************************************************/
tbLocker::tbLocker(QMainWindow *_mw, tbTextEdit_Handler *_textedit, tbStatusBar_Handler *_statusbar) : 
    m_mainwindow(_mw), 
    m_statusbar(_statusbar), 
    m_textedit(_textedit), 
    m_header(0),
    m_diary_file_read(false),
    m_have_key(false)
{ 
    //go find the users home directory
    QStringList pathList = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    //There should be 1 home directory!
    for (QStringList::iterator it = pathList.begin(); it!=pathList.end(); ++it)
    { 
        m_homepath = *it;
    } 

    //see if they already have a diary
    // For now, we will just note whether a diary exists, so we can decrypt
    //  upon opening it later if it does exist (we will need to use keyDialog
    //  to get the key before decrypting
    //  - the other scenario is if a file doesn't currently exist, in that case
    //    we will just note that here, and if they save something we will ask for
    //    a key and create a new file with that entry.
    //   
    m_diary_filename = m_homepath + "/" + tbLocker::DIARY_FILE_NAME;
    m_diary_filename = QDir::toNativeSeparators(m_diary_filename);

    m_diary_file = new QFile(m_diary_filename);
    if (m_diary_file->exists() == true)
    {   //it already exists
        m_file_exists = true;
    }
    else
    {
        m_file_exists = false;
    }

}

/**********************************************************************
 * tbLocker copy constructor - private
 * 
 * non-copyable 
 * 
 *
 *********************************************************************/
tbLocker::tbLocker(const tbLocker&) : QObject()
{
}

/**********************************************************************
 * tbLocker assignment operator - private
 * 
 * non-copyable 
 * 
 *
 *********************************************************************/
tbLocker& tbLocker::operator = (const tbLocker&)
{
    return *this;
}

/**********************************************************************
 * tbLocker destructor 
 * 
 * close the file 
 * 
 *
 *********************************************************************/
tbLocker::~tbLocker()
{
    m_diary_file->close();
}


void
tbLocker::about(void)
{
    QMessageBox msgBox(m_mainwindow);
    QString msg = QString("tbPrivaryDiary version %1.\nYour encrypted diary is stored in HOME/private_diary.pdl\ndiaryEncryption: Blowfish by Bruce Schneier").arg(DIARY_VERSION);
    msgBox.setText(msg);
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);

    msgBox.exec();
}

/**********************************************************************
 * tbLocker::create_new_diary_file 
 * 
 * creates a new diary file 
 * writes header, flushes, and closes it
 *
 *********************************************************************/
bool
tbLocker::create_new_diary_file(void)
{
    //QString header = tbLocker::DIARY_FILE_HEADER + DIARY_VERSION;
    QByteArray ba = (tbLocker::DIARY_FILE_HEADER + DIARY_VERSION).toUtf8();
    encrypt_buffer(&ba); 
    bool l_file_is_open = m_diary_file->open(QIODevice::ReadWrite);
    if (true == l_file_is_open)
    {
        m_diary_file->write((char*)ba.data(), ba.size());
        m_diary_file->flush();
        m_diary_file->close();
        m_header = new diary_header_t;
        m_header->header = ba;
    }

    m_diary_file->close();
    return l_file_is_open;
}

/**********************************************************************
 * tbLocker::new_entry menu item handler - obsolete
 * 
 * 
 * 
 *
 *********************************************************************/
void 
tbLocker::new_entry(void)
{
    m_textedit->new_entry();
}

/**********************************************************************
 * tbLocker::open_entry menu item handler
 * 
 * opens an entry specified by datetime 
 * 
 *
 *********************************************************************/
void 
tbLocker::open_entry(void)
{
    //They would be wanting to open
    //   by date time.
    diary_entry_t list_entry;

    //if we haven't read the file yet, read it
    if (false == m_diary_file_read)
        read_diary_file();

    //go get a datestring from the selectEntry dialog
    QString entry_str = get_datestring();
    QDateTime entry_dt;
    if (entry_str.isEmpty())
        return;
    else
    {
        //make a QDateTime from the entry
        entry_dt = QDateTime::fromString(entry_str, "ddd MMM dd hh:mm:ss yyyy"); 
    }
    
    
    QByteArray decrypt_buf;
    QDateTime temp_dt;
    QByteArray entry_decrypt_buf;
    //find the entry and set it in the m_textedit handler
    QListIterator<diary_entry_t> it(m_diary_entry_list);
    //use QListIterator, since it provides only const access
    while (it.hasNext())
    {
       list_entry=it.next();
       decrypt_buf = list_entry.datetime;
       decrypt_buffer(&decrypt_buf);
       QString dt_string(decrypt_buf.data());
       temp_dt = QDateTime::fromString(dt_string, "ddd MMM dd hh:mm:ss yyyy");
       if (temp_dt == entry_dt)
       {
           decrypt_buf = list_entry.entry_text;
           decrypt_buffer(&decrypt_buf);
           QString entry_text_string(decrypt_buf.data());
           QString entry_string = entry_dt.toString() + tbLocker::DATETIME_SPACER_STRING + entry_text_string;
           m_textedit->set_entry_text(entry_string);
           return;
       }
    }   
    
}

/**********************************************************************
 * tbLocker::application_close_msg
 * 
 * close the app, this happens if we can't get a good key from user  
 * 
 *
 *********************************************************************/
void
tbLocker::application_close_msg(void)
{
    QMessageBox msgBox(m_mainwindow);
    msgBox.setText("The application will be closed.");
    msgBox.exec();
    //exit(1);
    app_quit();
}

/**********************************************************************
 * tbLocker::save_entry menu item handler
 * 
 * saves the current entry
 * 
 *
 *********************************************************************/
void 
tbLocker::save_entry(void)
{
    //1.  They want to save the entry they are
    //    currently working on
    bool save_entry = true;

    if (false == m_have_key)
    {
        get_key();
        if (false == m_have_key)
        {
             return;
        }
    }

    //check the pdl file
    if (false == m_file_exists)
    {
        create_new_diary_file();
    }

    bool valid_file = read_diary_file();
    if (false == valid_file)
    {
        application_close_msg();
    }


    //dump_entries();

    //go get the text from the textbox
    QString text = m_textedit->get_text();

    // We need to pull apart the datetime and the remainging text is the entry_text
    //the first "tbLocker::DATETIME_STRING_LENGTH" chars are the date time string
    QString datestring; int i=0;
    for (QString::iterator it=text.begin() ; it!=text.end(); ++it)
    { 
        datestring.push_back(*it); 
        ++i;
        if (i==tbLocker::DATETIME_STRING_LENGTH) break;
    } 
    QDateTime datetime = QDateTime::fromString(datestring, "ddd MMM dd hh:mm:ss yyyy");
    //isolate the text from the datetime tag
    text = text.remove(0, tbLocker::DATETIME_STRING_LENGTH + tbLocker::DATETIME_SPACER_LENGTH);
    //put them into byte arrays
    QByteArray ba_text = text.toUtf8();
    QByteArray ba_datetime = datestring.toUtf8();

    //Here we want to check if we already have this entry, or if saving a new entry is it not empty
    diary_entry_t *d_entry;
    if (true == entry_exists(datetime, &d_entry))
    {
        encrypt_buffer(&ba_text); 
        d_entry->entry_text = ba_text;
    }
    else if (true == text.isEmpty())
    {
        //QMessage to inform user entry is empty
        save_entry = false;
    }
    else
    {
        diary_entry_t* diary_entry = new diary_entry_t;

        //START TAG - UNENCRYPTED
        diary_entry->start_tag = tbLocker::DIARY_ENTRY_START_TAG.toUtf8(); 

        //DATETIME 
        encrypt_buffer(&ba_datetime);
        diary_entry->datetime = ba_datetime;
    
        //ENTRY_TEXT
        encrypt_buffer(&ba_text); 
        diary_entry->entry_text = ba_text;

        //END TAG - UNENCRYPTED
        diary_entry->end_tag = tbLocker::DIARY_ENTRY_END_TAG.toUtf8(); 

        
        add_entry_to_list(diary_entry);
    }

    if (true == save_entry) 
        save_diary_file();

    //dump_entries();
}

/**********************************************************************
 * tbLocker::add_entry_to_list
 * 
 * adds an entry to the list
 * 
 *
 *********************************************************************/
void 
tbLocker::add_entry_to_list(diary_entry_t* list_entry)
{
    m_diary_entry_list.append(*list_entry);
}

/**********************************************************************
 * tbLocker::insert_into_entry_list
 * 
 * adds an entry to the list
 * 
 *
 *********************************************************************/
//void 
//tbLocker::insert_into_entry_list(diary_entry_t* list_entry)
//{
    //list_entry->next = m_diary_entry_list;
    //m_diary_entry_list = list_entry;
//}

/**********************************************************************
 * tbLocker::entry_exists
 * 
 * determines if an entry already exists
 * 
 *
 *********************************************************************/
bool
tbLocker::entry_exists(QDateTime dt, diary_entry_t **d_entry)
{
    QByteArray decrypt_buf;
    QDateTime temp_dt;
   
    //Here we need to use a QList iterator, since we need to get a pointer to the data and pass it back to caller
    //  (As opposed to a QListIterator which only allows const access)
    for (QList<diary_entry_t>::iterator it=m_diary_entry_list.begin(); it!=m_diary_entry_list.end(); ++it)
    {
       decrypt_buf = it->datetime;
       decrypt_buffer(&decrypt_buf);
       QString dt_string(decrypt_buf.data());
       temp_dt = QDateTime::fromString(dt_string, "ddd MMM dd hh:mm:ss yyyy");
       if (temp_dt == dt)
       {
           *d_entry = &*it; 
           return true;
       }
    }
    return false;
}

/**********************************************************************
 * tbLocker::save_diary_file
 * 
 * writes all the entries to the diary file
 * 
 *
 *********************************************************************/
void
tbLocker::save_diary_file(void)
{
    QByteArray temp_array;
    bool l_file_is_open = m_diary_file->open(QIODevice::ReadWrite);
    if (true == l_file_is_open)
    {
        m_diary_file->resize(0); 
        m_diary_file->seek(0); 
        //write header
        m_diary_file->write((char*)m_header->header.data(), m_header->header.size());

        //write entries
        QListIterator<diary_entry_t> it(m_diary_entry_list);
        diary_entry_t list_entry;
        while (it.hasNext()) 
        {
            list_entry=it.next();
            m_diary_file->write((char*)list_entry.start_tag.data(), list_entry.start_tag.size());
            m_diary_file->write((char*)list_entry.datetime.data(), list_entry.datetime.size());
            m_diary_file->write((char*)list_entry.entry_text.data(), list_entry.entry_text.size());
            m_diary_file->write((char*)list_entry.end_tag.data(), list_entry.end_tag.size());
        }
        
        m_diary_file->flush();
        m_diary_file->close();
    }
}


/**********************************************************************
 * tbLocker::reenter_key_mbox
 * 
 * after a bad key entry, asks if user wants to reenter key 
 * 
 *
 *********************************************************************/
bool
tbLocker::reenter_key_mbox(int retries_left)
{
    QMessageBox msgBox(m_mainwindow);
    QString msg = QString("The key appears to be invalid, reenter (%1 retries left)?").arg(retries_left);
    msgBox.setText(msg);
    QPushButton *yesButton = msgBox.addButton(tr("Yes"), QMessageBox::ActionRole);
    QPushButton *noButton = msgBox.addButton(QMessageBox::Abort);

    msgBox.exec();

    if (msgBox.clickedButton() == (QAbstractButton*)yesButton) {
       return true;
    } else if (msgBox.clickedButton() == (QAbstractButton*)noButton) {
       return false; 
    }
    else
        return false;
}

/**********************************************************************
 * tbLocker::read_diary_file
 * 
 * reads the diary file into the list of entries 
 * 
 *
 *********************************************************************/
bool
tbLocker::read_diary_file(void)
{
    QByteArray entry_text;
    bool l_file_is_open = false;

    if (true == m_diary_file_read)
        return true;

    //if we got here because the user did 'open' first thing
    //  we will need to check if we have a key and if a file
    //  already exists or not
    if (false == m_have_key)
    {
        get_key();
        if (false == m_have_key)
        {
             return false;
        }
    }

    //check the pdl file
    if (false == m_file_exists)
    {
        create_new_diary_file();
    }

    //Ok, we can open the file at this point
    l_file_is_open = m_diary_file->open(QIODevice::ReadWrite);

    if (true == l_file_is_open)
    {   //read the header
        QByteArray hd;
        m_diary_file->seek(0);
        hd = m_diary_file->read(tbLocker::DIARY_BLOCK_SIZE_BYTES*tbLocker::DIARY_HEADER_BLOCKS); 
        if (hd.size() != tbLocker::DIARY_BLOCK_SIZE_BYTES*tbLocker::DIARY_HEADER_BLOCKS)
        {
            //qDebug() << "Something went wrong reading the file!";
            return false;
        }

        QByteArray saved_buffer = hd;
        bool key_good = verify_diary_header(&saved_buffer); 

        if (true == key_good)
        {   //the key decrypts the header
            if (0 == m_header)
            {   
                m_header = new diary_header_t;
                m_header->header = hd;
            }
        }
        else
        {
            return false;
        }

        
        //read entries 
        QByteArray ba;
        while (1)
        {
            ba = m_diary_file->read(tbLocker::DIARY_BLOCK_SIZE_BYTES*tbLocker::DIARY_START_TAG_BLOCKS);
            if (ba.size() != tbLocker::DIARY_BLOCK_SIZE_BYTES*tbLocker::DIARY_START_TAG_BLOCKS)
            {
                break;
            }
            else 
            {
                diary_entry_t* diary_entry = new diary_entry_t;
                diary_entry->start_tag = ba;
                //read 4 blocks, the datetime
                diary_entry->datetime = m_diary_file->read(tbLocker::DIARY_BLOCK_SIZE_BYTES*tbLocker::DIARY_DATETIME_BLOCKS);
                qint64 file_position = m_diary_file->pos(); 
                bool end_tag_found = false;
                entry_text.clear();
                //move byte-by-byte, reading 2 blocks at a time looking for end tag
                while (false == end_tag_found)
                {
                    ba = m_diary_file->read(tbLocker::DIARY_BLOCK_SIZE_BYTES*tbLocker::DIARY_END_TAG_BLOCKS);
                    if (ba.size() == tbLocker::DIARY_BLOCK_SIZE_BYTES*tbLocker::DIARY_END_TAG_BLOCKS)
                    { //this shouldn't be a problem since the file should always end with END TAG BLOCK 
                    }
                    else
                    {
                      //problem reading file
                    }
 
                    if ((ba.data() == tbLocker::DIARY_ENTRY_END_TAG))
                    {

                        diary_entry->entry_text = entry_text;
                        diary_entry->end_tag = ba;
                        add_entry_to_list(diary_entry);

                        end_tag_found = true;
                    
                    } 
                    else
                    {
                        //seek to position and read one byte
                        m_diary_file->seek(file_position);
                        ba = m_diary_file->read(1);
                        entry_text.append(ba);  
                        file_position++;
                    }
                }
            }
        }
    }

    m_diary_file->close();
    m_diary_file_read = true;
    return true;
}

/**********************************************************************
 * tbLocker::get_key
 * 
 * gets the diary key from the user 
 * 
 *
 *********************************************************************/
bool 
tbLocker::get_key(void)
{

    keyDialog kd(m_mainwindow);
    kd.setWindowModality(Qt::ApplicationModal);
    kd.exec();

    if (true == kd.get_text(&m_file_key))
    {
        m_have_key = true;
        m_key_byte_array = m_file_key.toLocal8Bit();
        m_key_array = m_key_byte_array.data();
        blf_key(&m_c, (quint8*)m_key_array, m_file_key.length());

    }
    else
    {
        m_have_key = false;
        m_file_key = "";
    }

    return m_have_key;
}

/**********************************************************************
 * tbLocker::get_datestring
 * 
 * gets the diary key from the user 
 * 
 *
 *********************************************************************/
QString
tbLocker::get_datestring(void)
{
    selectEntryDialog sd(m_mainwindow);
    sd.setWindowModality(Qt::ApplicationModal);

    QByteArray decrypt_buf;
    QDateTime temp_dt;
    QString datestring;
    diary_entry_t list_entry;
   

    //Use QListIterator here to prevent modification of
    //underlying data.  I *believe* QListIterator only
    //provides const access
    QListIterator<diary_entry_t> it(m_diary_entry_list);
    while (it.hasNext())
    {
       list_entry=it.next();
       decrypt_buf = list_entry.datetime;
       decrypt_buffer(&decrypt_buf);
       QString dt_string(decrypt_buf.data());
       sd.add_item(dt_string);
    }

    sd.exec();
    
    return (datestring = sd.get_selection());

}

/**********************************************************************
 * tbLocker::encrypt_buffer
 * 
 * encrypts the buffer  
 * caller places data there and calls here
 *
 *********************************************************************/
bool 
tbLocker::encrypt_buffer(QByteArray* arr)
{
    int str_length = arr->length();
    int buffer_size = str_length;
    buffer_size = (buffer_size + 7)&~7; 
    //if buffer size is still equal to str_length, then
    //   str_length was already a multiple of 8
    //   so we add a pad of 8 (a block)
    int pad;
    if (buffer_size == str_length)
    {
        pad = 8;
    }
    else
       pad = buffer_size-str_length;  //pad = 1-7

    char c = (char)(pad+48);  //any number in ascii is the number + 48

    for (int i=0; i<pad; ++i)
    {
        arr->append(c);
    }

    blf_enc(&m_c, (quint32*)arr->data(), arr->size()/tbLocker::DIARY_BLOCK_SIZE_BYTES);
    return true;

}


/**********************************************************************
 * tbLocker::decrypt_buffer
 * 
 * decrypts the buffer  
 * caller places data there and calls here
 *
 *********************************************************************/
bool 
tbLocker::decrypt_buffer(QByteArray* arr)
{
    blf_dec(&m_c, (quint32*)arr->data(), arr->size()/8);
    //remove the padding, use QByteArray.chop(n);
    QByteArray end = arr->right(1);
    char *data = end.data();
    int pad_bytes = (int)(data[0] - 48);
    //pad bytes will be 1-8 on a valid encrypted buffer.
    //If not, then likely the key is no good
    if ((pad_bytes > 0) && (pad_bytes < 9))
    {
        arr->chop(pad_bytes);
        return true;
    }
    else
    {
       return false;
    }
}


/**********************************************************************
 * tbLocker::verify_diary_header
 * 
 * decrypts the buffer (class member m_byte_buffer) 
 * caller places encrypted header data there and calls here
 *
 * user is given so many retries to get password right
 *********************************************************************/
bool
tbLocker::verify_diary_header(QByteArray *arr)
{
    bool key_good = false;
    int key_retries = 3;
    //make a copy of the original array
    QByteArray saved_buffer = *arr;


    decrypt_buffer(&saved_buffer);

    if (saved_buffer.contains(tbLocker::DIARY_FILE_HEADER.toStdString().c_str()))
    {
        return true;
    }
    else
    {
        while ((false == key_good) && (--key_retries > 0))
        {
            bool ret = reenter_key_mbox(key_retries);
            if (true == ret)
            {
                bool got_key = get_key();
                if (true == got_key)
                {
                    //refresh the saved buffer and try to verify again
                    saved_buffer = *arr;
                    decrypt_buffer(&saved_buffer);

                    if (saved_buffer.contains(tbLocker::DIARY_FILE_HEADER.toStdString().c_str()))
                    {
                        return true;
                    }
                }
                else
                { //they cancelled 
                    return false;
                }
            }
            else
                return false;
        }
        return false;
    }
}



/**********************************************************************
 * tbLocker::write_status
 * 
 * writes to the status bar 
 * 
 *
 *********************************************************************/
void
tbLocker::write_status(QString str)
{
    if (m_statusbar)
        m_statusbar->update_status_bar(str);
        
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
////////////////////// DEBUGGING //////////////////////////////////////

/**********************************************************************
 * tbLocker::dump_entries
 * 
 * for debugging dumps the entry list 
 *
 * 
 *********************************************************************/
void
tbLocker::dump_entries(void)
{
    QByteArray decrypt_buf;
    diary_entry_t list_entry;
    int entry_num = 0; 

    std::cout << "DUMP ENTRIES:" << std::endl;

    QListIterator<diary_entry_t> it(m_diary_entry_list);
    while (it.hasNext())
    {
       list_entry=it.next();
       decrypt_buf = list_entry.datetime;
       decrypt_buffer(&decrypt_buf);
       std::cout << "ENTRY #" << ++entry_num << ": datetime: " << decrypt_buf.data();

       decrypt_buf = list_entry.entry_text;
       decrypt_buffer(&decrypt_buf);
       std::cout << "ENTRY #" << entry_num << ": entry_text: " << decrypt_buf.data();
    }
}



/**********************************************************************
 * tbLocker::test_encryption
 * 
 * for debugging tests encryption  
 *
 * 
 *********************************************************************/
void
tbLocker::test_encryption(void)
{
    QByteArray ba = tbLocker::DIARY_ENTRY_START_TAG.toUtf8();

    std::cout << "TEST_ENC: ba initially <" << ba.data() << ">" << std::endl;
    encrypt_buffer(&ba);
    decrypt_buffer(&ba);
    std::cout << "TEST_ENC: ba finally <" << ba.data() << ">" << std::endl;

    
}


