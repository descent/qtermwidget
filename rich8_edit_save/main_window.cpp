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

#include <QtGui>
#include <QActionGroup>
#if 1
#include <QMessageBox>
#include <QComboBox>
//#include <QPoint>
#include <QLineEdit>
#endif


#include <QtGlobal> // for qVersion()
#include <QtDebug> // for qDebug()



#include <cstdio>
#include <cstdlib>

using namespace std;

#include "main_window.h"

//const u32 CARD_OFFSET_1P = 0x4e38;
u32 CARD_OFFSET_1P = 0x4e2c;
const u32 CASH_DIFF = 0x4e38-0x4e14; // cash, 4 bytes
const u32 SAVING_DIFF = CASH_DIFF+0x4; // saving, 4 bytes
const u32 POINT_DIFF = 0x4e38-0x4e10; // point, 2bytes

const u32 PLAYER_DIFF = 0x5024-0x4e38;
const u32 CARD_OFFSET_2P = CARD_OFFSET_1P + PLAYER_DIFF;
const u32 CARD_OFFSET_3P = CARD_OFFSET_2P + PLAYER_DIFF;
const u32 CARD_OFFSET_4P = CARD_OFFSET_3P + PLAYER_DIFF;

const u32 OFFSET[]={
	            0x4e20, // last stage, 2P
                    0x4e2c,
                    0x4e38,
                   };

#ifdef Q_OS_WIN32
const QString config_fn="\\.rich8_edit_save.cfg";
#else
const QString config_fn="/.rich8_edit_save.cfg";
#endif
const int CARD_NUM=50;
//const char *card_name[CARD_NUM]={"購地", "建屋"};
const char *card_name[]={
"空白",  // 00 00 00 00, if ff ff ff ff use the field
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
"隱形", // 0x31 00 00 00
"未知", // 0x32 00 00 00, wrong data, this is not card data.
};

const char *persion_name[]={
"烏咪",
"錢夫人",
"大老千",
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

//MainWindow::MainWindow():QMainWindow(),fs_(0), backup_(false)
MainWindow::MainWindow():QMainWindow(),fs_(0)
{ 
  setWindowIcon(QIcon(":/images/window_icon.png"));
  create_form_groupbox();
  open_rich8_cfg();
  persion_data_[0]= OFFSET[0];
  persion_data_[1]= persion_data_[0] + PLAYER_DIFF;
  persion_data_[2]= persion_data_[1] + PLAYER_DIFF;
  persion_data_[3]= persion_data_[2] + PLAYER_DIFF;

  // init font
  QDomNodeList nodes=dom_doc_.elementsByTagName("ui_font");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.
  bool ok=false;
  QFont f(e.attribute("family"), e.attribute("pointSize").toInt(&ok, 10), e.attribute("weight").toInt(&ok, 10));
  f.setStyle((QFont::Style)e.attribute("style").toInt(&ok, 10));
  qDebug() << "ok: " << ok;

  setFont(f);

  setCentralWidget(formGroupBox);
  file_menu_ = menuBar()->addMenu(tr("&File"));
  ADD_ACTION(file_menu_, open_file_, "&Open File", open_file_slot )
  ADD_ACTION(file_menu_, save_file_, "&Save", save_file_slot )
  ADD_ACTION(file_menu_, save_as_, "Save &As", save_as_slot )

  setting_menu_ = menuBar()->addMenu(tr("&Setting"));
  ADD_ACTION(setting_menu_, change_font_, "&Font", change_font_slot);
  ADD_ACTION(setting_menu_, backup_file_, "&Backup File", backup_file_slot);
  backup_file_->setCheckable(true);

  nodes=dom_doc_.elementsByTagName("backup_file");
  e = nodes.at(0).toElement(); // try to convert the node to an element.

  if (e.attribute("option")=="y")
    backup_file_->setChecked(true);
  else
    backup_file_->setChecked(false);

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

  qDebug() << "QDir::currentPath(): " << QDir::currentPath();
  qDebug() << "QDir::homePath(): " << QDir::homePath();
  //QString QDir::toNativeSeparators ( const QString & pathName )   [static]



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

void MainWindow::closeEvent ( QCloseEvent * event )
{
  QDomNodeList nodes=dom_doc_.elementsByTagName("backup_file");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.

  if (backup_file_->isChecked())
  {
    qDebug("y");
    e.setAttribute("option", "y");
  }
  else
  {
    qDebug("n");
    e.setAttribute("option", "n");
  }
  QString cfg_fn_path=QDir::homePath() + config_fn;

  QFile file(cfg_fn_path);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream out(&file);
    qDebug() << "dom_doc_: " << dom_doc_.toString();
    out << dom_doc_;
    file.close();
  }


  event->accept();

#if 0
  if (maybeSave()) {
    writeSettings();
    event->accept();
  } else 
  {
    event->ignore();
  }
#endif
}

void MainWindow::open_rich8_cfg()
{
  QString cfg_fn_path=QDir::homePath() + config_fn;
  qDebug() << "cfg_fn_path: " << cfg_fn_path;
  if (QFile::exists (cfg_fn_path))
  {
    QFile file(cfg_fn_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return;

    dom_doc_.setContent(&file);
    file.close();

    QString xml_txt = dom_doc_.toString();
    qDebug() << "xml_txt: " << xml_txt;
    //QDomElement tag=dom_doc_.elementById("font");
    //QDomNodeList tag=dom_doc_.elementsByTagName("font");
    QDomNodeList tag=dom_doc_.elementsByTagName("backup_file");

#if 0
    for (int i=0 ; i < tag.length() ; ++i)
    {
      QDomNode n=tag.at(i);

      QDomElement e = tag.at(i).toElement(); // try to convert the node to an element.
      qDebug() << "e: " << e.text();
      qDebug() << "tag.at(i).nodeName(): " << tag.at(i).nodeName();

      qDebug() << "childNodes().length(): " << n.childNodes().length();
      QDomNode old_node=n.childNodes().at(0);
      QDomText new_node=dom_doc_.createTextNode("no");
      n.replaceChild(new_node, old_node);

      #if 0
      qDebug() << "aaa e: " << n.nodeValue();
      n.setNodeValue("no");
      qDebug() << "xxx e: " << n.nodeValue();
      #endif
    }
#endif
  } // The file does not exist, create dom doc
  else
  {
    QDomElement root = dom_doc_.createElement("rich8_cfg");
    dom_doc_.appendChild(root);
  
    QDomElement tag = dom_doc_.createElement("cfg_path");
    root.appendChild(tag);
  
    QDomText t = dom_doc_.createTextNode(cfg_fn_path);
    tag.appendChild(t);

#if 0
    tag = dom_doc_.createElement("ui_font_pointSize");
    root.appendChild(tag);
  
    t = dom_doc_.createTextNode("ui_font_pointSize");
    tag.appendChild(t);
  
    tag = dom_doc_.createElement("ui_font_weight");
    root.appendChild(tag);
  
    t = dom_doc_.createTextNode("ui_font_weight");
    tag.appendChild(t);

    tag = dom_doc_.createElement("ui_font_style");
    root.appendChild(tag);
  
    t = dom_doc_.createTextNode("ui_font_style");
    tag.appendChild(t);
#endif
    tag = dom_doc_.createElement("ui_font");
    root.appendChild(tag);

    tag.setAttribute("family", "f1");
    tag.setAttribute("pointSize", "p1");
    tag.setAttribute("weight", "w1");
    tag.setAttribute("style", "s1");
  
    t = dom_doc_.createTextNode("ui_font");
    tag.appendChild(t);


    tag = dom_doc_.createElement("backup_file");
    root.appendChild(tag);
  
    t = dom_doc_.createTextNode("yes");
    tag.appendChild(t);
  
    tag = dom_doc_.createElement("open_file_path");
    root.appendChild(tag);
    t = dom_doc_.createTextNode(".");
    tag.appendChild(t);
  
#if 0
    QString xml = dom_doc_.toString();
    qDebug() << "xml: " << xml;

     QFile file(cfg_fn_path);
     if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return;

     QTextStream out(&file);
     out << doc;
  file.close();
#endif
  }

}

bool MainWindow::close()
{
  return QWidget::close();
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

   // ref : Qt-4.6.2/examples/xml/dombookmarks/mainwindow.cpp
   QMessageBox::about(this, tr("About Rich8 Save File Editor"), msg);

#if 0


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
  #endif
}

const int BUF_LEN=32;
//const  int offset=0x4e30+8; // 1P begin card offset

#define QT_FILE_IO

void MainWindow::open_file_slot()
{
#ifdef QT_FILE_IO
#else
  if (fs_)
    fclose(fs_);
#endif

  QDomNodeList nodes=dom_doc_.elementsByTagName("open_file_path");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.

  if (dirname_.isNull())
  {
    dirname_= e.attribute("path");
  }
  

  file_name_ = QFileDialog::getOpenFileName(this, tr("Open Rich8 save"), dirname_);
  if (file_name_.isNull()) return;
  dirname_=file_name_.left(file_name_.lastIndexOf("/"));
  basename_=file_name_.right(file_name_.size()-file_name_.lastIndexOf('/')-1);

  qf_.setFileName(file_name_);

#ifdef QT_FILE_IO
  if (qf_.open(QIODevice::ReadOnly))
  {
  }
#else
  if ( (fs_=fopen(file_name_.toStdString().c_str(), "rb+") ))
  {
  }
#endif
  else
  {
#if 0
    QMessageBox msg_box;

    msg_box.setText("Cannot open:" + file_name_);
    msg_box.exec();
#endif


    // ref : Qt-4.6.2/examples/xml/dombookmarks/mainwindow.cpp
    QMessageBox::warning(this, tr("rich8 save editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file_name_)
                             .arg(qf_.errorString()));


    return;
  }

  qba_=qf_.readAll();
  size_t read_count=qba_.size();
  if (read_count ==0) 
  {
    QMessageBox msg_box;

    msg_box.setText(basename_ + " size is 0." );
    msg_box.exec();
    return; // should not happen
  }
  qf_.close();

  formGroupBox->setTitle(file_name_);

  e.setAttribute("path", dirname_);

  statusBar()->showMessage(tr("open"));
  fill_data(persion_data_[0]);
}

void MainWindow::backup_file()
{
}

// offset is card offset
void MainWindow::fill_data(int offset)
{
  qDebug() << hex << "offset : " << offset;
  size_t read_count=10;
  QString result;
#ifdef QT_FILE_IO
  const char *buf = qba_.constData();
#endif

#ifndef QT_FILE_IO
  u8 buf[BUF_LEN]="";
  fseek(fs_, offset, SEEK_SET);
  read_count=fread(buf, 1, 1, fs_);
#endif

// point data
    //QTextStream(&result) << buf[0];
    u8 u8_data=0;
    u16 u16_data=0;
    memcpy(&u16_data, buf+offset-POINT_DIFF, 2);
    result.sprintf("%d", u16_data);

    point_->setText(result);

#ifdef QT_FILE_IO
#else
    fseek(fs_, offset, SEEK_SET);
    read_count=fread(buf, 1, 4, fs_);
#endif

// cash data
    int u32_data=0;
    memcpy(&u32_data, buf+offset-CASH_DIFF, 4);
    //QTextStream(&result) << buf[0];
    result.sprintf("%d", u32_data);

    cash_->setText(result);


#ifdef QT_FILE_IO
#else
    fseek(fs_, offset, SEEK_SET);
    read_count=fread(buf, 1, 4, fs_);
#endif

// saving data
    //offset+=4;
    //QTextStream(&result) << buf[0];
    memcpy(&u32_data, buf+offset-CASH_DIFF+0x4, 4);
    result.sprintf("%d", u32_data);

    saving_->setText(result);

#ifdef QT_FILE_IO
#else
    fseek(fs_, offset, SEEK_SET);
    read_count=fread(buf, 1, BUF_LEN, fs_);
#endif

// card data
    //offset=0x4e38;
    //offset=0x4e2c;
  //qDebug() << "read_count: " << read_count;
  const char *card_data=buf+offset;
  qDebug() << hex << "card data offset: " << offset;
  for (size_t i=0,j=0 ; i < 32 ; i+=4, ++j)
  {
    u32 u32_data=0;
    memcpy(&u32_data, card_data+i, 4);
    qDebug("card_data[%d]: %x", i, card_data[i]);
    qDebug() << hex << "card u32 data: " << u32_data;
    #if 0
    qDebug("buf[%d]: %x", i+1, buf[i+1]);
    qDebug("buf[%d]: %x", i+2, buf[i+2]);
    qDebug("buf[%d]: %x", i+3, buf[i+3]);
    #endif
    // buf[i]-1 0 ~ sizeof(card_name)/sizeof(char*)
    //u8 ch=card_data[i];
    //if (buf[i]==0xffffffff)
    if (u32_data==0xffffffff)
    {
      card_[j]->setCurrentIndex(0);
    }
    else if (0 <= (u32_data-1) && (u32_data-1) <= (sizeof(card_name)/sizeof(char*)-2) )
         {
           card_[j]->setCurrentIndex(u32_data);
         }
         else // unknow card data
	 {
           qDebug() << "unknow card data";
           card_[j]->setCurrentIndex(sizeof(card_name)/sizeof(char*)-1);
#if 0
           card_[i]->addItem(tr("unknow card (%1 %2 %3 %4)")
			     .arg((int)card_data[0], 0, 16)
			     .arg((int)card_data[1], 0, 16)
			     .arg((int)card_data[2], 0, 16)
			     .arg((int)card_data[3], 0, 16) );
#endif
	 }
  }

}

void MainWindow::save_as_slot()
{
  backup_fn_= file_name_ + ".bak";
  file_name_ = QFileDialog::getSaveFileName(this, tr("Open Rich8 to Save As"), dirname_);
  if (file_name_.isNull()) return;
  dirname_=file_name_.left(file_name_.lastIndexOf("/"));
  basename_=file_name_.right(file_name_.size() - file_name_.lastIndexOf('/')-1);

#ifdef QT_FILE_IO
  save_file_slot();
#endif // QT_FILE_IO
}

int MainWindow::write_to_save_file(const QString &w_fn)
{
  QFile qf;

  qf.setFileName(w_fn);
  if (!qf.open(QIODevice::WriteOnly))
  {
#if 0
    QMessageBox msg_box;

    msg_box.setText("Cannot open:" + file_name_ + " to write");
    msg_box.exec();
#endif

    QMessageBox::warning(this, tr("rich8 save editor"),
                             tr("Cannot save file %1:\n%2.")
                             .arg(w_fn)
                             .arg(qf.errorString()));


    return -1;
  }
  qDebug() << "qba_.size() : " << qba_.size();
  int w_len=qf.write(qba_);
  qDebug() << "w_len : " << w_len;
  qf.close();
  return 0;
}

void MainWindow::save_file_slot()
{
  int offset = persion_data_[players_->currentIndex()];
  
  if (backup_fn_.isEmpty())
    backup_fn_= file_name_ + ".bak";

  if (backup_file_->isChecked())
  {
    qDebug() << "backup file: " << backup_fn_;
    write_to_save_file(backup_fn_);
  }
  // point data
  char *buf=qba_.data();
  QString qstr=point_->text();
  bool ok;
  u16 u16_data = qstr.toInt(&ok, 10);
  memcpy(buf+offset-POINT_DIFF, &u16_data, 2);
  //qDebug() << "point_->text(): " << point_->text();
  //qDebug() << "u16_data: " << u16_data;

// cash data
    qstr=cash_->text();
    int u32_data = qstr.toInt(&ok, 10);
    memcpy(buf+offset-CASH_DIFF, &u32_data, 4);

    // saving data
    qstr=saving_->text();
    u32_data = qstr.toInt(&ok, 10);
    memcpy(buf+offset-CASH_DIFF+0x4, &u32_data, 4);

// card data
  //offset=0x4e38;
  //offset=0x4e2c; // 舞美拉 3p
  //offset=CARD_OFFSET_1P;
  buf=qba_.data() + offset;

  qDebug() << hex << "write offset: " << offset;

  // write card data
  for (size_t j=0 ; j < MAX_CARD_NUM ; ++j)
  {
    //char write_buf[4]="";
    //size_t w_len=0;
    int idx=card_[j]->currentIndex();
    if (idx==0)
      buf[3] = buf[2] = buf[1] = buf[0]=0xff;
    else
      buf[0]=idx, buf[1] = buf[2] = buf[3] = 0;
    buf+=4;
  }

  // wirte to file
  write_to_save_file(file_name_);

#ifdef QT_FILE_IO

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
  formGroupBox->setTitle(file_name_);
  statusBar()->showMessage(tr("save"));
  backup_fn_="";
}

void MainWindow::change_save_file_offset ( int index )
{
  //qDebug() << "MainWindow::change_save_file_offset";
  //CARD_OFFSET_1P = OFFSET[index];
  persion_data_[0]= OFFSET[index];
  persion_data_[1]= persion_data_[0] + PLAYER_DIFF;
  persion_data_[2]= persion_data_[1] + PLAYER_DIFF;
  persion_data_[3]= persion_data_[2] + PLAYER_DIFF;

  for (int i=0 ; i < 4; ++i)
    qDebug() << hex << "persion_data_[" << i << "] : "  << persion_data_[i];

  qDebug() << hex << "persion_data_[" << players_->currentIndex() << "] : "  << persion_data_[players_->currentIndex()];
  fill_data(persion_data_[players_->currentIndex()]);
}

void MainWindow::change_player ( int index )
{
  qDebug() << "player: " << index;
  fill_data(persion_data_[players_->currentIndex()]);
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


  save_file_offset_ = new QComboBox(this);
  for (int i=0 ; i < sizeof(OFFSET)/sizeof(sizeof(u32)) ; ++i)
  {
    save_file_offset_->addItem(QString("0x%1").arg(OFFSET[i], 0, 16));
  }
  QObject::connect(save_file_offset_, SIGNAL(currentIndexChanged ( int )), this, SLOT(change_save_file_offset(int)));
#if 0
  save_file_offset->addItem("0x4e20");
  save_file_offset->addItem("0x4e2c");
  save_file_offset->addItem("0x4e38");
#endif
  layout->addRow(new QLabel(tr("card offset")), save_file_offset_);


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

  for (size_t i=0 ; i < sizeof(persion_name)/sizeof(char*) ; ++i)
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
      else if (j==(sizeof(card_name)/sizeof(char*)-1)) // last unknow card
           {
              QTextStream(&result) << "(" << hex << j << " 00 00 00)";
           }
           else
	   {
              QTextStream(&result) << "(" << hex << j << " 00 00 00)";
	   }

      cn = codec->toUnicode(card_name[j]) + codec->toUnicode("卡") + result;
      card_[i]->addItem(cn);
    }
#endif
  }

}


void MainWindow::backup_file_slot()
{
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

  QDomNodeList nodes=dom_doc_.elementsByTagName("ui_font");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.
  e.setAttribute("family", terminal_font.family());
  e.setAttribute("pointSize", terminal_font.pointSize());
  e.setAttribute("weight", terminal_font.weight());
  e.setAttribute("style", terminal_font.style());

  #if 0
  for (int i=0 ; i < nodes.length() ; ++i)
  {
    QDomElement e = nodes.at(i).toElement(); // try to convert the node to an element.
    qDebug() << "e: " << e.text();
    //e.text()
  }

    qDebug() << "terminal_font.family()" << terminal_font.family();
    qDebug() << "terminal_font.pointSize()" << terminal_font.pointSize();
    qDebug() << "terminal_font.weight()" << terminal_font.weight();
    qDebug() << "terminal_font.style()" << terminal_font.style();
  #endif
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
