/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

//! [0]
#include <QtGui>
#include <QActionGroup>
#include <QMessageBox>
#include <QtGlobal> // for qVersion()
#include <QComboBox>
#include <QtDebug> // for qDebug()
//#include <QPoint>
#include <QLineEdit>


#include "main_window.h"



#include <cstdio>
#include <cstdlib>


using namespace std;

const u32 PERSION_DATA[4]={0x4e10, 0x4ffc, 0x51e8, 0x53d4};
const int CARD_NUM=50;
//const char *card_name[CARD_NUM]={"購地", "建屋"};
const char *card_name[]={
"空白", 
"購地", 
"建屋", 
"拆屋", 
"交換", 
"查封",  // 05 00 00 00
"怪獸",
"機車",
"汽車",
"遙控骰子",
"烏龜",  // 0a 00 00 00
"轉向",  // 0b
"路障",  // 0c
"停留", // 0d
"機器娃娃", // 0e
"冬眠", // 0f
"地雷", // 0x10 00 00 00
"定時炸彈", // 0x11 00 00 00
"飛彈", // 0x12 00 00 00
"核子飛彈", // 0x13 00 00 00
"均貧", // 0x14 00 00 00
"均富", // 0x15 00 00 00
"黑", // 0x16 00 00 00
"紅", // 0x17 00 00 00
"嫁禍", // 0x18 00 00 00
"搶奪", // 0x19 00 00 00
"送神", // 0x1a 00 00 00
"查稅", // 0x1b 00 00 00
"陷害", // 0x1c 00 00 00
"漲價", // 0x1d 00 00 00
"天使", // 0x1e 00 00 00
"換位", // 0x1f 00 00 00
"竹蜻蜓", // 0x20 00 00 00
"泡泡", // 0x21 00 00 00
"路霸", // 0x22 00 00 00
"封印", // 0x23 00 00 00
"財神", // 0x24 00 00 00
"窮神", // 0x25 00 00 00
"衰神", // 0x26 00 00 00
"福神", // 0x27 00 00 00
"土地公", // 0x28 00 00 00
"糊塗神", // 0x29 00 00 00
"警察", // 0x2a 00 00 00
"小偷", // 0x2b 00 00 00
"野狗", // 0x2c 00 00 00
"老千", // 0x2d 00 00 00
"保鏢", // 0x2e 00 00 00
"鑽石", // 0x2f 00 00 00
"地王", // 0x30 00 00 00
"", // 0x31 00 00 00
"隱形", // 0x32 00 00 00
};

const char *persion_name[]={
"烏咪",
"錢夫人",
};


#define ADD_ACTION(menu, qa_obj, qa_name, slot) \
{ \
  qa_obj = new QAction(tr(qa_name), this); \
  connect(qa_obj, SIGNAL(triggered()), this, SLOT(slot())); \
  menu->addAction(qa_obj); \
}

#define SET_KEY_SHORTCUT(keybind, qa_obj) \
{ \
  qa_obj->setShortcut(QKeySequence(keybind)); \
}


void f(){
  //QTextCodec *codec = QTextCodec::codecForName("utf8");
  //QByteArray data = codec->fromUnicode(msgContent);
//将msgContent转换成QByteArray,在接收时使用

  //card_name[0]=a;
}

MainWindow::MainWindow():QMainWindow(),fs_(0)
{
  //card_value_.insert(std::pair<u32, QString>(0x01000000, "abc"));
  create_form_groupbox();
  setCentralWidget(formGroupBox);
  file_menu_ = menuBar()->addMenu(tr("&File"));
  ADD_ACTION(file_menu_, open_file_, "&Open File", open_file_slot )
  ADD_ACTION(file_menu_, save_file_, "&Save", save_file_slot )
  ADD_ACTION(file_menu_, save_as_, "Save &As", save_as_slot )

  setting_menu_ = menuBar()->addMenu(tr("&Setting"));
  ADD_ACTION(setting_menu_, change_font_, "&Font", change_font_slot);

  help_menu_ = menuBar()->addMenu(tr("&Help"));
  ADD_ACTION(help_menu_, about_, "&About", about_slot)

  QRect desktop_rect=QApplication::desktop()->screenGeometry();

  qDebug() << desktop_rect; // for debug

  //int w=desktop_rect.width()/2;
  //int h=desktop_rect.height()/2;

  //move(desktop_rect.center());
  QPoint p=QApplication::desktop()->screen()->rect().center() - centralWidget()->rect().center();
  qDebug() << p;
  p = QApplication::desktop()->screen()->rect().center();
  qDebug() << QApplication::desktop()->screen()->rect().center();
  qDebug() << centralWidget()->rect().center();
  qDebug() << centralWidget()->width();
  qDebug() << centralWidget()->height();
  qDebug() << width();
  qDebug() << height();
  move(p.x()-width()/2, p.y()-height()/2);
  //statusBar()->showMessage(tr("Select a save file"));

  //setGeometry(desktop_rect);
  //qDebug() << desktop_rect.topLeft();
  //qDebug << desktop_rect.bottomRight();
#if 0
  //QList<QKeySequence> key_shortcuts;

  SET_KEY_SHORTCUT(tr("F2"), new_tab_);

  ADD_ACTION(file_menu_, close_tab_, "&close tab", close_tab_slot )
  SET_KEY_SHORTCUT(tr("F3"), close_tab_);

  edit_menu_ = menuBar()->addMenu(tr("&Edit"));

  ADD_ACTION(edit_menu_, copy_, "&copy", copy_slot)
  ADD_ACTION(edit_menu_, paste_, "&paste", paste_slot)

  //Qt::Key_Control	0x01000021	On Mac OS X, this corresponds to the Command keys.
  //Qt::Key_Meta	0x01000022	On Mac OS X, this corresponds to the Control keys. On Windows keyboards, this key is mapped to the Windows key.
  
#if defined(Q_OS_MAC)
  SET_KEY_SHORTCUT(Qt::CTRL + Qt::Key_C, copy_);
  SET_KEY_SHORTCUT(Qt::CTRL + Qt::Key_V, paste_);
#else // win, linux/x use windows key
  SET_KEY_SHORTCUT(Qt::META + Qt::Key_C, copy_);
  SET_KEY_SHORTCUT(Qt::META+ Qt::Key_V, paste_);
  //SET_KEY_SHORTCUT("Ctrl+Shift+C", copy_);
  //SET_KEY_SHORTCUT("Ctrl+Shift+V", paste_);
#endif

  SET_KEY_SHORTCUT(tr("Ctrl+B"), change_bg_color_);
  SET_KEY_SHORTCUT(tr("Ctrl+F"), change_font_);

  encoding_menu_ = menuBar()->addMenu(tr("E&ncoding") );

  ADD_ACTION(encoding_menu_, big5_enc_, "BIG5", big5_enc)
  ADD_ACTION(encoding_menu_, utf8_enc_, "UTF8", utf8_enc)


  SET_KEY_SHORTCUT(tr("Ctrl+5"), big5_enc_);
  SET_KEY_SHORTCUT(tr("Ctrl+8"), utf8_enc_);

  input_enc_g_ = new QActionGroup(this);
  enc_g_ = new QActionGroup(this);
  enc_g_->addAction(big5_enc_);
  enc_g_->addAction(utf8_enc_);

  big5_enc_->setCheckable(true);
  utf8_enc_->setCheckable(true);

  tab_widget_ = new QTabWidget;

  setCentralWidget(tab_widget_);

  for (int i=0 ; i < 6 ; ++i)
  {
    new_tab_slot();
  }
  //tab_widget_->setFocus();
#endif
}

#if defined(Q_OS_MAC)
const char *mac_version_str()
{
  switch (QSysInfo::MacintoshVersion)
  {
    case QSysInfo::MV_10_4 :
    {
      return "10.4";
    }
    case QSysInfo::MV_10_5 :
    {
      return "10.5";
    }
    case QSysInfo::MV_10_6 :
    {
      return "10.6";
    }
    default:
    {
      return "unknow";
    }
  }
}
#endif

#ifdef Q_OS_WIN32
const char *win_version_str()
{
#if 1
  switch ( QSysInfo::windowsVersion() )
  {
    case QSysInfo::WV_5_0:
    {
      return "windows 2000";
    }
    case QSysInfo::WV_5_1:
    {
      return "windows xp";
    }
    case QSysInfo::WV_6_0:
    {
      return "windows vista";
    }
    case QSysInfo::WV_6_1:
    {
      return "windows 7";
    }
    default:
    {
      return "unknow";
    }
  }
  #endif
}
#endif

void MainWindow::about_slot()
{
  QMessageBox msg_box;
  QString msg;
#ifdef Q_OS_WIN32
  QSysInfo::windowsVersion ()  ;
  msg.sprintf("%s\nQT version: %s", win_version_str(), qVersion());
  //QApplication::winVersion () ; // old version
#elif defined(Q_OS_LINUX)
  msg.sprintf("Linux/X\nQT version: %s", qVersion());
#elif defined(Q_OS_MAC)
  QSysInfo::MacintoshVersion;
  //msg+="\nmac osX"  ;
  msg.sprintf("mac osX %s\nQT version: %s", mac_version_str(), qVersion());
#endif
  //msg+=" version";

  msg_box.setText(msg);
  qDebug() << centralWidget()->geometry().center();
  //msg_box.move(centralWidget()->geometry().center());
  //msg_box.move(centralWidget()->geometry().center() - rect().center());
  //QRect desktop_rect=QApplication::desktop()->screenGeometry();
  //msg_box.move(desktop_rect.center());
  qDebug() << "geometry()" << geometry();
  qDebug() << "rect()" << rect();
  int w=msg_box.geometry().width();
  int h=msg_box.geometry().height();
  qDebug() << "w: " << w;
  qDebug() << "h: " << h;
  qDebug() << "rect().center" << rect().center();
  //msg_box.move(geometry().center().x()-w/2, geometry().center().y()-h/2);
  //msg_box.move(geometry().x()+w/2, geometry().y()+h/2);
  msg_box.move(geometry().x()+158/2, geometry().y()+95/2);
  msg_box.exec();
  w=msg_box.geometry().width();
  h=msg_box.geometry().height();
  qDebug() << "w: " << w;
  qDebug() << "h: " << h;
  //qDebug("about");  	
}

const int BUF_LEN=32;
//const  int offset=0x4e30+8; // 1P begin card offset

#define QT_FILE_IO

void MainWindow::open_file_slot()
{
  static QString dirname, basename;
#ifdef QT_FILE_IO
  qf_.close();
#else
  if (fs_)
    fclose(fs_);
#endif
  QString file_name = QFileDialog::getOpenFileName(this, tr("Open Rich8 save"), dirname);
  qDebug() << "file_name.lastIndexOf('\') " << file_name.lastIndexOf('/');
  dirname=file_name.left(file_name.lastIndexOf("/"));
  qDebug() << "xx file_name.lastIndexOf('\') " << file_name.lastIndexOf('/');
  basename=file_name.right(file_name.size()-file_name.lastIndexOf('/')-1);
  qDebug() << "dirname: " << dirname;
  qDebug() << "basename: " << basename;
  qDebug() << file_name;
  if (file_name.isNull()) return;

  qf_.setFileName(file_name);

#ifdef QT_FILE_IO
  if (qf_.open(QIODevice::ReadWrite))
  {
  }
#else
  if ( (fs_=fopen(file_name.toStdString().c_str(), "rb+") ))
  {
  }
#endif
  else
  {
    QMessageBox msg_box;

    msg_box.setText("Cannot open:" + file_name);
    msg_box.exec();
    return;
  }
  formGroupBox->setTitle(file_name);
  statusBar()->showMessage(tr("open"));
  backup_file();
  fill_data(PERSION_DATA[0]);
}

void MainWindow::backup_file()
{
}

void MainWindow::fill_data(int offset)
{
  size_t read_count=10;
  QString result;
#ifdef QT_FILE_IO
  char *buf;
  QByteArray qba;
#else
  u8 buf[BUF_LEN]="";
#endif

// point data
#ifdef QT_FILE_IO
    qf_.seek(offset);
    qba=qf_.read(1);
    buf=qba.data();
#else
    fseek(fs_, offset, SEEK_SET);
    read_count=fread(buf, 1, 1, fs_);
#endif
    //QTextStream(&result) << buf[0];
    result.sprintf("%d", buf[0]);

    point_->setText(result);

// cash data
    offset=0x4e14;
#ifdef QT_FILE_IO
    qf_.seek(offset);
    qba=qf_.read(4);
    buf=qba.data();
#else
    fseek(fs_, offset, SEEK_SET);
    read_count=fread(buf, 1, 4, fs_);
#endif
    int u32_data=0;
    memcpy(&u32_data, buf, 4);
    //QTextStream(&result) << buf[0];
    result.sprintf("%d", u32_data);

    cash_->setText(result);


// saving data
    offset+=4;
#ifdef QT_FILE_IO
    qf_.seek(offset);
    qba=qf_.read(4);
    buf=qba.data();
#else
    fseek(fs_, offset, SEEK_SET);
    read_count=fread(buf, 1, 4, fs_);
#endif
    //QTextStream(&result) << buf[0];
    memcpy(&u32_data, buf, 4);
    result.sprintf("%d", u32_data);

    saving_->setText(result);

// card data
    offset=0x4e38;
#ifdef QT_FILE_IO
    qf_.seek(offset);
    qba=qf_.read(BUF_LEN);
    read_count=qba.size();
    buf=qba.data();
#else
    fseek(fs_, offset, SEEK_SET);
    read_count=fread(buf, 1, BUF_LEN, fs_);
#endif

  for (size_t i=0,j=0 ; i < read_count ; i+=4, ++j)
  {
    //qDebug("buf[i]: %x", buf[i]);
    // buf[i]-1 0 ~ sizeof(card_name)/sizeof(char*)
    if (0 <= (buf[i]-1) && (buf[i]-1) <= (sizeof(card_name)/sizeof(char*)-1) )
      if (buf[0]==0xff)
        card_[j]->setCurrentIndex(0);
      else
        card_[j]->setCurrentIndex(buf[i]);
  }

}

void MainWindow::save_as_slot()
{
}

void MainWindow::save_file_slot()
{
  int offset = PERSION_DATA[players_->currentIndex()];

// card data
  offset=0x4e38;
#ifdef QT_FILE_IO

  qf_.seek(offset);

  qDebug() << "write offset: " << offset;
  for (size_t j=0 ; j < MAX_CARD_NUM ; ++j)
  {
    char write_buf[4]="";
    size_t w_len=0;
    int idx=card_[j]->currentIndex();
    if (idx==0)
      write_buf[3] = write_buf[2] = write_buf[1] = write_buf[0]=0xff;
    else
      write_buf[0]=idx;
    w_len=qf_.write(write_buf, 4);
    //qDebug() << "w_len: " << w_len;
  }
  qf_.close();

#else

  if (fs_==0)
    return;	   

  fseek(fs_, offset, SEEK_SET);

  for (size_t j=0 ; j < MAX_CARD_NUM ; ++j)
  {
    u8 write_buf[4]="";
    size_t w_len=0;
    int idx=card_[j]->currentIndex();
    write_buf[0]=idx+1;
    w_len=fwrite(write_buf, 1, 4, fs_);
  }
  fclose(fs_);
  fs_=0;
#endif
  statusBar()->showMessage(tr("save"));
}

void MainWindow::change_player ( int index )
{
  qDebug() << "player: " << index;

}

// copy from: 
//      /usr/local/Trolltech/Qt-4.6.2/examples/layouts/basiclayouts/dialog.cpp
void MainWindow::create_form_groupbox()
{
  QTextCodec *codec = QTextCodec::codecForName("utf8");
  formGroupBox = new QGroupBox(tr("Select a save file"));
  QFormLayout *layout = new QFormLayout;
  formGroupBox->setLayout(layout);
  QString label_name; // card name utf8 encoding
#if 0
  for (int i=0; i < MAX_PERSION ; ++i)
  {
    persion_[i] = new QComboBox(this);
    QString result;
    QTextStream(&result) << i+1 << "P: ";
    //label_name = result + codec->toUnicode(" P:");
    layout->addRow(new QLabel(result), persion_[i]);
  }
#endif

  players_ = new QComboBox(this);
  persion_ = new QComboBox(this);
  players_->addItem("1P");
  players_->addItem("2P");
  players_->addItem("3P");
  players_->addItem("4P");

  //connect(players_, currentIndexChanged ( int ), this,  );
  QObject::connect(players_, SIGNAL(currentIndexChanged ( int )), this, SLOT(change_player(int)));

  for (int i=0 ; i < sizeof(persion_name)/sizeof(char*) ; ++i)
  {
      QString cn; // card name utf8 encoding

      cn = codec->toUnicode(persion_name[i]);
      persion_->addItem(cn);
  }
  layout->addRow(players_, persion_);

  //layout->setSpacing(1);

  cash_ = new QLineEdit(this);
  saving_ = new QLineEdit(this);
  point_ = new QLineEdit(this);
  position_ = new QLineEdit(this);
  direction_ = new QLineEdit(this);

  layout->addRow(new QLabel("cash"), cash_);
  layout->addRow(new QLabel("saving"), saving_);
  layout->addRow(new QLabel("point"), point_);
  layout->addRow(new QLabel("position"), position_);
  layout->addRow(new QLabel("direction"), direction_);
  //layout->addRow(new QLabel("save"), card_[i]);

  for (int i=0; i < MAX_CARD_NUM ; ++i)
  {

    card_[i] = new QComboBox(this);
    //layout->addRow(new QLabel(tr("Line 1:")), new QLineEdit);
    label_name = codec->toUnicode("卡片 ");

    QString result;

    QTextStream(&result) << i+1;
    //layout->addRow(new QLabel((label_name)), card_[i]);
    layout->addRow(new QLabel(label_name+result), card_[i]);
    //layout->addRow(new QLabel(tr("Line 3:")), new QSpinBox);
#if 1
    for (size_t j=0 ; j < sizeof(card_name)/sizeof(char*) ; ++j)
    {
      QString cn; // card name utf8 encoding
      QString result;

      if (j==0)
        QTextStream(&result) << "(ff ff ff ff)";
      else
        QTextStream(&result) << "(" << hex << j << " 00 00 00)";

      cn = codec->toUnicode(card_name[j]) + codec->toUnicode("卡") + result;
      card_[i]->addItem(cn);
    }
#endif
  }

}


void MainWindow::change_font_slot()
{
  bool ok;

  qDebug("change_font_slot");
  #if 0
  QColor color = QColorDialog::getColor(Qt::black, this);
  if (color.isValid())
  {
    (dynamic_cast<QTermWidget *> (tab_widget_->currentWidget()))->get_terminal_display()->set_background_color(color);
  }
  #endif


  static QFont terminal_font;
  terminal_font = QFontDialog::getFont(&ok, terminal_font ,this);
  //QFont terminal_font = QFontDialog::getFont(&ok, QFont("Monospace", 14) ,this);

  if (ok)
  {
    setFont(terminal_font);
  }
  else
  {
    qDebug("not ok");
  }

}

#if 0
void MainWindow::switch_tab_slot(int tab_index)
{
  //qDebug("tab_index: %d", tab_index);
  if (tab_index < tab_widget_->count())
    tab_widget_->setCurrentIndex(tab_index);
}



void MainWindow::copy_slot()
{
  qDebug("copy_slot");
  //(dynamic_cast<QTermWidget *>(centralWidget()))->copy_to_clipboard();
  if (tab_widget_->currentWidget())
    (dynamic_cast<QTermWidget *> (tab_widget_->currentWidget()))->copy_to_clipboard();
}

void MainWindow::paste_slot()
{
  qDebug("paste_slot");
  //(dynamic_cast<QTermWidget *>(centralWidget()))->paste_from_clipboard();
  if (tab_widget_->currentWidget())
    (dynamic_cast<QTermWidget *> (tab_widget_->currentWidget()))->paste_from_clipboard();
}

void MainWindow::big5_enc()
{
  static QTextCodec *codec=QTextCodec::codecForName("big5");

  if (codec==0)
  {
    qWarning() << "can not get big5 codec" << endl;
    return;
  }

  qDebug("big5");
  //(dynamic_cast<QTermWidget *>(centralWidget()))->set_codec(codec);
  if (tab_widget_->currentWidget())
    (dynamic_cast<QTermWidget *> (tab_widget_->currentWidget()))->set_codec(codec);

  big5_enc_->setChecked(true);
}
void MainWindow::utf8_enc()
{
  qDebug("utf8");
  static QTextCodec *codec=QTextCodec::codecForName("utf8");

  if (codec==0)
  {
    qWarning() << "can not get utf8 codec" << endl;
    return;
  }

  //(dynamic_cast<QTermWidget *>(centralWidget()))->set_codec(codec);
  if (tab_widget_->currentWidget())
    (dynamic_cast<QTermWidget *> (tab_widget_->currentWidget()))->set_codec(codec);
  utf8_enc_->setChecked(true);
}

bool MainWindow::close()
{
  qDebug("MainWindow::close()");
  if (tab_widget_->count()==1)
    return QWidget::close();
  else 
  {
    tab_widget_->removeTab(tab_widget_->currentIndex());
    for (int i=0 ; i < tab_widget_->count() ; ++i)
    {
      char num_str[5];

      sprintf(num_str, "%d", i+1);
      tab_widget_->setTabText(i, num_str);
    }
  }


  return false;
}

QTermWidget *MainWindow::create_qterm_widget()
{
  QTermWidget *console = new QTermWidget();

  //QFont font = QApplication::font();
  QFont font;
  //QFont font = QFont("Monospace", 14);
  //font.setFamily("Terminus");
#ifdef Q_OS_WIN32
  font.setFamily("Monospace");
  font.setPointSize(14);
  QSysInfo::windowsVersion ()  ;
  QApplication::winVersion () ; // old version
#elif defined(Q_OS_LINUX)
  font.setFamily("Monospace");
  font.setPointSize(14);
#elif defined(Q_OS_MAC)
  font.setFamily("Andale Mono");
  font.setPointSize(18);
  //QApplication::macVersion () ; // old verison
  //QSysInfo::macVersion ()  ;
  QSysInfo::MacintoshVersion;
  
#endif
  //font.setFamily("efont");
  //font.setPointSize(11);
    
  //QFont font = QFont("Osaka-等幅", 18);
  console->setTerminalFont(font);
  console->get_terminal_display()->set_background_color(QColor(57,57,57));
  console->setScrollBarPosition(QTermWidget::ScrollBarRight);
  //console->setFocus();
  //QApplication::setActiveWindow(console);

  QObject::connect(console, SIGNAL(finished()), this, SLOT(close()));
  QObject::connect(console->get_terminal_display(), SIGNAL(switch_tab(int)), this, SLOT(switch_tab_slot(int)));
  //console->get_terminal_display()->setFocus();
  return console;
}

void MainWindow::close_tab(int tab_index)
{
  char tab_index_str[6];

  qDebug("tab_index: %d", tab_index);
  tab_widget_->removeTab(tab_index);

  for (int i=tab_index ; i < tab_widget_->count() ; ++i)
  {
    sprintf(tab_index_str, "%d", i+1);
    tab_widget_->setTabText(i, QString(tab_index_str));
  }

}

void MainWindow::close_tab_slot()
{
  if (tab_widget_->count() != 1)
    close_tab(tab_widget_->currentIndex());
}


#if 0
void TerminalTabWidget::keyPressEvent(QKeyEvent* event)
{
  //if (event->modifiers() & Qt::MetaModifier) 
  if (event->modifiers() & Qt::AltModifier) 
  {
    if (event->key() == Qt::Key_1)
      qDebug("win+1");

  }
  qDebug("MainWindow::keyPressEvent");
  

  QTabWidget::keyPressEvent(event);
}
#endif

void MainWindow::set_encode_slot()
{
#if 0
  static int i = 0;

	
  if (i)
  {
    (dynamic_cast<QTermWidget *>(centralWidget()))->set_codec(QTextCodec::codecForName("big5"));
    qDebug("big5");
  }
  else
  {
    qDebug("utf8");
  }
  ++i;
#endif
}

void MainWindow::change_bg_color_slot()
{
  QColor color = QColorDialog::getColor(Qt::black, this);
  if (color.isValid())
  {
  #ifdef CHANGE_CURRENT_QTERMWIDGET
    (dynamic_cast<QTermWidget *> (tab_widget_->currentWidget()))->get_terminal_display()->set_background_color(color);
  #else
    for (int i=0 ; i < tab_widget_->count() ; ++i) {
      (dynamic_cast<QTermWidget *> (tab_widget_->widget(i)))->get_terminal_display()->set_background_color(color);
      (dynamic_cast<QTermWidget *> (tab_widget_->widget(i)))->get_terminal_display()->updateImage();
    }
  #endif
  }
}




#if 0

#include "main_window.h"
#include "qtermwidget.h"


//! [0]

//! [1]
MainWindow::MainWindow():QMainWindow()
//! [1] //! [2]
{
    textEdit = new QTextEdit;
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    setCurrentFile("");
}
#if 0
MainWindow::~MainWindow()
{
    //delete (centralWidget());
}
#endif
//! [2]

//! [3]
void MainWindow::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}
//! [4]

//! [5]
void MainWindow::newFile()
//! [5] //! [6]
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile("");
    }
}
//! [6]

//! [7]
void MainWindow::open()
//! [7] //! [8]
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}
//! [8]

//! [9]
bool MainWindow::save()
//! [9] //! [10]
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}
//! [10]

//! [11]
bool MainWindow::saveAs()
//! [11] //! [12]
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}
//! [12]

//! [13]
void MainWindow::about()
//! [13] //! [14]
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

#if 0
void MainWindow::change_font_slot()
{
  //extern QTermWidget *console;

  bool ok;

  qDebug("change_font_slot");

  static QFont terminal_font = QFontDialog::getFont(&ok, this);

  if (ok)
  {
    (dynamic_cast<QTermWidget *>(centralWidget()))->setTerminalFont(terminal_font);
    //QTermWidget *console = (QTermWidget *)centralWidget();
    //console->setTerminalFont(font_);

    qDebug("ok");
  }
  else
  {
    qDebug("not ok");
  }

}
#endif

//! [14]

//! [15]
void MainWindow::documentWasModified()
//! [15] //! [16]
{
    setWindowModified(textEdit->document()->isModified());
}
//! [16]

//! [17]
void MainWindow::createActions()
//! [17] //! [18]
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

//! [19]
    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
//! [18] //! [19]

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

//! [20]
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
//! [20]
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

//! [21]
    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
//! [21]
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

//! [22]
    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
//! [22]

//! [23]
    cutAct->setEnabled(false);
//! [23] //! [24]
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    //change_font_ = new QAction(tr("&Font"), this);
    //connect(change_font_, SIGNAL(triggered()), this, SLOT(change_font_slot()));
}
//! [24]

//! [25] //! [26]
void MainWindow::createMenus()
//! [25] //! [27]
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
//! [28]
    fileMenu->addAction(openAct);
//! [28]
    fileMenu->addAction(saveAct);
//! [26]
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    //setting_menu_ = menuBar()->addMenu(tr("&Setting"));
    //setting_menu_->addAction(change_font_);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}
//! [27]

//! [29] //! [30]
void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
//! [29] //! [31]
    fileToolBar->addAction(openAct);
//! [31]
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}
//! [30]

//! [32]
void MainWindow::createStatusBar()
//! [32] //! [33]
{
    statusBar()->showMessage(tr("Ready"));
}
//! [33]

//! [34] //! [35]
void MainWindow::readSettings()
//! [34] //! [36]
{
    QSettings settings("Trolltech", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}
//! [35] //! [36]

//! [37] //! [38]
void MainWindow::writeSettings()
//! [37] //! [39]
{
    QSettings settings("Trolltech", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}
//! [38] //! [39]

//! [40]
bool MainWindow::maybeSave()
//! [40] //! [41]
{
    if (textEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}
//! [41]

//! [42]
void MainWindow::loadFile(const QString &fileName)
//! [42] //! [43]
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}
//! [43]

//! [44]
bool MainWindow::saveFile(const QString &fileName)
//! [44] //! [45]
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
//! [45]

//! [46]
void MainWindow::setCurrentFile(const QString &fileName)
//! [46] //! [47]
{
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Application")));
}
//! [47]

//! [48]
QString MainWindow::strippedName(const QString &fullFileName)
//! [48] //! [49]
{
    return QFileInfo(fullFileName).fileName();
}
//! [49]
#endif

#endif
