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
#include <QLineEdit>
#endif


#include <QtGlobal> // for qVersion()
#include <QtDebug> // for qDebug()



#include <cstdio>
#include <cstdlib>

#include <vector>

#include <iostream>

using namespace std;

#include "main_window.h"


#ifdef Q_OS_WIN32
const QString config_fn="\.gpx2map.cfg";
#else
const QString config_fn="/.gpx2map.cfg";
#endif

const char *colors[]=
{
  "#E60000",
  "#00E675",
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
  "#0000A0", // Dark Blue 	
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
  qDebug() << "bb:";
  create_form_groupbox();
  open_rich8_cfg();

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

}

const int BUF_LEN=32;
//const  int offset=0x4e30+8; // 1P begin card offset

#define QT_FILE_IO

void MainWindow::open_file_slot()
{
  QDomNodeList nodes=dom_doc_.elementsByTagName("open_file_path");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.

  if (dirname_.isNull())
  {
    dirname_= e.attribute("path");
  }
  

  //file_name_ = QFileDialog::getOpenFileName(this, tr("Open GPX"), dirname_);
  QStringList fn_list;
  fn_list = QFileDialog::getOpenFileNames(this, tr("Open GPX"), dirname_, "*.gpx");


  points_ = "";
  for (int i=0 ; i < fn_list.length() ; ++i)
  {
    file_name_=fn_list.at(i);
    if (file_name_.isNull()) break;

    dirname_=file_name_.left(file_name_.lastIndexOf("/"));
    basename_=file_name_.right(file_name_.size()-file_name_.lastIndexOf('/')-1);

    qf_.setFileName(file_name_);
    if (!qf_.open(QIODevice::ReadOnly))
    {
      // ref : Qt-4.6.2/examples/xml/dombookmarks/mainwindow.cpp
      QMessageBox::warning(this, tr("rich8 save editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file_name_)
                             .arg(qf_.errorString()));
      break;
    }


    QDomDocument doc("mydocument");

    if (!doc.setContent(&qf_)) 
    {
      qf_.close();
      break;
    }
    qf_.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();
    cout << "root tagname: " << qPrintable(docElem.tagName()) << endl; // the node really is an element.

    QDomNode n = docElem.firstChild();

    #if 0
    // produce the html code
    t = 1; trk_info[t] = [];
    trk_info[t]['name'] = '南港-萬芳醫院'; trk_info[t]['desc'] = ''; trk_info[t]['clickable'] = true;
    trk_info[t]['color'] = '#E60000'; trk_info[t]['width'] = 3; trk_info[t]['opacity'] = 0.8;
    trk_info[t]['outline_color'] = '#000000'; trk_info[t]['outline_width'] = 0; trk_info[t]['fill_color'] = '#E60000'; trk_info[t]['fill_opacity'] = 0;
    trk_segments[t] = [];
    #endif
    //cout << "[";
    //const char color[]="#E60000";
    points_ += QString("t = %1; trk_info[t] = []; \
    trk_info[t]['name'] = '%2'; trk_info[t]['desc'] = ''; trk_info[t]['clickable'] = true; \
    trk_info[t]['color'] = '%3'; trk_info[t]['width'] = 3; trk_info[t]['opacity'] = 0.8; \
    trk_info[t]['outline_color'] = '#000000'; trk_info[t]['outline_width'] = 0; trk_info[t]['fill_color'] = '#E60000'; trk_info[t]['fill_opacity'] = 0; \
    trk_segments[t] = [];").arg(i+1).arg("route_name").arg(colors[i]);

    points_ += "trk_segments[t].push({points:[";
    search_all(n);
    cout << "xxx" << endl;
    points_ += QString("]}); \
                \nGV_Draw_Track(t); \
	        t = %1; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});\n").arg(i+1);
#if 0
    t = 1; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});
    t = 2; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});

#endif
    //cout << "]" << endl;

  }









  //text_edit_->clear();
  //text_edit_->insertPlainText(points_);

  e.setAttribute("path", dirname_);
  statusBar()->showMessage(tr("open"));
}

void MainWindow::search_all(QDomNode &n)
{
  while(!n.isNull()) 
  {
    //cout << "aaa" << endl;
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull()) 
    {
      //cout << qPrintable(e.tagName()) << endl; // the node really is an element.
      if (e.tagName()=="trkpt")
      {
        //qDebug() << e.attribute("lat");
        //qDebug() << e.attribute("lon");
	points_ += ("[" + e.attribute("lat") + "," + e.attribute("lon") + "],");
	//cout << "[" << qPrintable(e.attribute("lat")) << "," << qPrintable(e.attribute("lon")) << "]," << endl;

	//text_edit_->insertPlainText(e.attribute("lat"));
	//text_edit_->insertPlainText(e.attribute("lon"));
      }
#if 0
	 QDomNodeList nl=n.childNodes();
	 cout << "nl.length() : " << nl.length() << endl;
#else
         QDomNode node = e.firstChild();
         if(!node.isNull()) 
	 {
           //cout << "!node.isNull()" << endl;
           search_all(node);
	 }
	 else
	 {
           //cout << "node.isNull()" << endl;
           //return;
	 }
#endif
     }
     else
     {
       //cout << "e.isNull()" << endl;
       return;
     }


     //cout << "n.nextSibling()" << endl;
     n = n.nextSibling();
     //n = n.firstChild();
  }
  return;
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
  QFile template_file;

  QString template_fn="template.html";
  template_file.setFileName(template_fn);
  if (!template_file.open(QIODevice::ReadOnly))
  {
    QMessageBox::warning(this, tr("gpx2map"),
                             tr("Cannot open file %1:\n%2.")
                             .arg(template_fn)
                             .arg(template_file.errorString()));

    return -1;
  }

  QByteArray template_data=template_file.readAll();
  int pos=0;
  pos=template_data.indexOf("@@");
  qDebug() << "pos: " << pos;
  template_data.replace(pos, 2, points_.toAscii());
  template_file.close();

  qf.setFileName(w_fn);
  if (!qf.open(QIODevice::WriteOnly))
  {
    QMessageBox::warning(this, tr("gpx2map"),
                             tr("Cannot save file %1:\n%2.")
                             .arg(w_fn)
                             .arg(qf.errorString()));


    return -1;
  }
  int w_len=qf.write(template_data);
  qf.close();
  return 0;
}

void MainWindow::save_file_slot()
{
  if (backup_fn_.isEmpty())
    backup_fn_= file_name_ + ".bak";

  if (backup_file_->isChecked())
  {
    qDebug() << "backup file: " << backup_fn_;
    write_to_save_file(backup_fn_);
  }

  // wirte to file
  write_to_save_file(file_name_);

}

void MainWindow::change_save_file_offset ( int index )
{

}


// copy from: 
//      /usr/local/Trolltech/Qt-4.6.2/examples/layouts/basiclayouts/dialog.cpp
void MainWindow::create_form_groupbox()
{
  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
  formGroupBox = new QGroupBox(tr("Select a file"));

  formGroupBox->setLayout(layout);

  text_edit_ = new QTextEdit(this);
  layout->addWidget(text_edit_);
}


void MainWindow::backup_file_slot()
{
}

void MainWindow::change_font_slot()
{
  bool ok;

  qDebug("change_font_slot");


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





#endif
