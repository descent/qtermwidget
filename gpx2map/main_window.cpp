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

#include <unistd.h>


#include <QtGlobal> // for qVersion()
#include <QtDebug> // for qDebug()



#include <cstdio>
#include <cstdlib>


#include <iostream>

using namespace std;

#include "main_window.h"
#include "browserwindow.hpp"


#ifdef Q_OS_WIN32
const QString config_fn="\\.gpx2map.cfg";
#else
const QString config_fn="/.gpx2map.cfg";
#endif



const char *colors[]=
{
  "#E60000",
  "#00E675",
  "#0000A0", // Dark Blue 	
  "#FF0080", // Light Purple 	
  "#FFFF00", // Yellow	
  "#00FF00", // Pastel Green	
  "#0000FF", // Light Blue	
  "#FF00FF", // Pink	
  "#800080", // Dark Purple 	
  "#FFFFFF", //White	
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

bool qcolor2html_color_str(const QColor &qc, QString &html_color_str)
{
  QTextStream out(&html_color_str);
  out << hex << qc.rgb();
  html_color_str.replace(0, 2, "#");
  return true;
}

MainWindow::MainWindow():QMainWindow(), previous_fn_index_(0), browser_(0)
{ 
  setWindowIcon(QIcon(":/images/window_icon.png"));
  open_cfg();
  create_form_groupbox();

  // init font
  QDomNodeList nodes=dom_doc_.elementsByTagName("ui_font");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.
  bool ok=false;
  QFont f(e.attribute("family"), e.attribute("pointSize").toInt(&ok, 10), e.attribute("weight").toInt(&ok, 10));
  f.setStyle((QFont::Style)e.attribute("style").toInt(&ok, 10));

  setFont(f);

  setCentralWidget(formGroupBox);
  //file_menu_ = menuBar()->addMenu(tr("&File"));
  //ADD_ACTION(file_menu_, open_file_, "&Open File", open_file_slot )
  //ADD_ACTION(file_menu_, save_file_, "&Save", save_file_slot )
  //ADD_ACTION(file_menu_, save_as_, "Save &As", save_as_slot )

  setting_menu_ = menuBar()->addMenu(tr("&Setting"));
  ADD_ACTION(setting_menu_, change_font_, "&Font", change_font_slot);
  ADD_ACTION(setting_menu_, show_debug_log_, "&Show Debug Log", show_debug_log_slot);
  //backup_file_->setCheckable(true);

  nodes=dom_doc_.elementsByTagName("backup_file");
  e = nodes.at(0).toElement(); // try to convert the node to an element.

  help_menu_ = menuBar()->addMenu(tr("&Help"));
  ADD_ACTION(help_menu_, about_, "&About", about_slot)

  QRect desktop_rect=QApplication::desktop()->screenGeometry();

  QPoint p=QApplication::desktop()->screen()->rect().center() - centralWidget()->rect().center();
  p = QApplication::desktop()->screen()->rect().center();
  move(p.x()-width()/2, p.y()-height()/2);

  qDebug() << "QDir::currentPath(): " << QDir::currentPath();
  qDebug() << "QDir::homePath(): " << QDir::homePath();
}

void MainWindow::closeEvent ( QCloseEvent * event )
{
  QDomNodeList nodes=dom_doc_.elementsByTagName("backup_file");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.

  nodes=dom_doc_.elementsByTagName("google_map_key");
  if (nodes.size()==0)
  { // create a tag
    qDebug() << "create google map key" << endl;
    QDomElement root = dom_doc_.createElement("cfg");
    QDomElement tag = dom_doc_.createElement("google_map_key");
    root.appendChild(tag);
    tag.setAttribute("k1", google_map_key_->text());
  }
  else
  {
    qDebug() << "modify google map key" << endl;
    e = nodes.at(0).toElement(); // try to convert the node to an element.
    e.setAttribute("k1", google_map_key_->text());
  }

  QString cfg_fn_path=QDir::homePath() + config_fn;

  QFile file(cfg_fn_path);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QTextStream out(&file);
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

void MainWindow::open_cfg()
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
    QDomNodeList tag=dom_doc_.elementsByTagName("backup_file");

  } 
  else
  { // The file does not exist, create dom doc
    QDomElement root = dom_doc_.createElement("cfg");
    dom_doc_.appendChild(root);
  
    QDomElement tag = dom_doc_.createElement("cfg_path");
    root.appendChild(tag);
  
    QDomText t = dom_doc_.createTextNode(cfg_fn_path);
    tag.appendChild(t);

    tag = dom_doc_.createElement("ui_font");
    root.appendChild(tag);

    tag.setAttribute("family", "f1");
    tag.setAttribute("pointSize", "p1");
    tag.setAttribute("weight", "w1");
    tag.setAttribute("style", "s1");
  
    t = dom_doc_.createTextNode("ui_font");
    tag.appendChild(t);

    tag = dom_doc_.createElement("google_map_key");
    root.appendChild(tag);

    tag.setAttribute("k1", "");


    tag = dom_doc_.createElement("backup_file");
    root.appendChild(tag);
  
    t = dom_doc_.createTextNode("yes");
    tag.appendChild(t);





    tag = dom_doc_.createElement("save_file_path");
    root.appendChild(tag);
    t = dom_doc_.createTextNode(".");
    tag.appendChild(t);
  
    tag = dom_doc_.createElement("open_file_path");
    root.appendChild(tag);
    t = dom_doc_.createTextNode(".");
    tag.appendChild(t);
  
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
  msg+="\nversion: $Rev$";


   // ref : Qt-4.6.2/examples/xml/dombookmarks/mainwindow.cpp
   QMessageBox::about(this, tr("About GPX2MAP"), msg);

}


#define QT_FILE_IO


FileTrkAttr* MainWindow::get_cur_file_trk_attr()
{
  //FileTrkAttr* cur_file_trk_attr = 0;
  if (file_trk_attr_.count(dirname_ + "/" + files_->currentText()))
  {
    //cur_file_trk_attr = file_trk_attr_[fn];
    return file_trk_attr_[dirname_ + "/" + files_->currentText()];
  }
  else
  {
    return 0;
  }
}

void MainWindow::get_trk(const QString &fn, int index)
{
  if (fn.isNull()) return;

  setCursor(Qt::WaitCursor);
  QFile qf;
  qf.setFileName(fn);
  if (!qf.open(QIODevice::ReadOnly))
  {
    // ref : Qt-4.6.2/examples/xml/dombookmarks/mainwindow.cpp
    QMessageBox::warning(this, tr("gpx2map"),
                               tr("Cannot read file %1:\n%2.")
                               .arg(fn)
                               .arg(qf.errorString()));
    return;
  }
  QDomDocument doc("gpx_file");
  if (!doc.setContent(&qf)) 
  {
    qf.close();
    QMessageBox::warning(this, tr("gpx2map"), tr("%1 is not a gpx format file.").arg(file_name_));
    return;
  }
  qf.close();

  QString tag_name=check_gpx_type(doc);
  QString trk_name;
  if (tag_name=="rtept")
    trk_name="rte";
  if (tag_name=="trkpt")
    trk_name="trk";

  QObject::disconnect(track_list_, SIGNAL(currentIndexChanged ( int )), this, SLOT(load_gpx_attr(int)));
  track_list_->clear();
  QDomNodeList node_list=doc.elementsByTagName(trk_name);

  FileTrkAttr* cur_file_trk_attr = 0;
  if (file_trk_attr_.count(fn))
  {
    cur_file_trk_attr = file_trk_attr_[fn];
  }
  else
  {
  }

  QStringList color_name=QColor::colorNames();
  cur_file_trk_attr->clear();
  for (int i=0 ; i < node_list.size() ; ++i)
  {
    MapAttribute trk_attr;
    QDomNode n = node_list.at(i).firstChild();
    //QDomNode node = e.firstChild();

    get_trk_name(n, trk_attr.name);
    qDebug() << "trk_attr.name: " << trk_attr.name;

    if(!n.isNull()) 
    {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if(!e.isNull()) 
      {
        //cout << qPrintable(e.tagName()) << endl; // the node really is an element.
        track_list_->addItem(trk_attr.name);
        //trk_attr.name=e.text();
	//qDebug() << e.text();
        trk_attr.color=color_index_%(sizeof(colors)/sizeof(char*));
        trk_attr.qc=color_name.at((color_index_+color_name.size())%color_name.size());

        RouteItem *item = new RouteItem(route_view_);
	if (trk_attr.name=="")
	  trk_attr.name="noname";

        item->setText(0, trk_attr.name);
        item->setText(1, trk_attr.name);
	QFileInfo fi(fn);
        item->setText(3, fi.fileName());
	//route_view_->editItem(item, 1);
	//item->setFlags(item->flags() | Qt::ItemIsEditable);

        //item->setText(1, "route color 1");

	//qDebug() << "(color_index_+color_name.size())%color_name.size(): " << (color_index_+color_name.size())%color_name.size();
	//qDebug() << "color_name.size(): " << color_name.size();
	//qDebug() << "color_index_:" << color_index_;
	color_index_+=19;

	// current code, points will do many copy time.
        get_trk_points(n, tag_name, trk_attr.points);
	//qDebug() << "trk_attr.points: " << trk_attr.points;
        cur_file_trk_attr->push_back(trk_attr);

	item->set_attr(trk_attr);
      }
    }
    //get_trk_points(n.firstChild(), tag_name);
  }

  QObject::connect(track_list_, SIGNAL(currentIndexChanged ( int )), this, SLOT(load_gpx_attr(int)));
  unsetCursor();
  parse_gpx_.insert(fn);

#if 0
  QDomElement docElem = doc.documentElement();
  QDomNode n = docElem.firstChild();

  get_trk_info(n, tag_name);
#endif

}

void MainWindow::get_trk_info(QDomNode &n, const QString &tag_name)
{
  while(!n.isNull()) 
  {
    //cout << "aaa" << endl;
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull()) 
    {
#if 1
      if ( (e.tagName()=="trk") 
		      || (e.tagName()=="name") || 
		      //(e.tagName()=="desc") || 
		      (e.tagName()=="trkseg") ) 
      {
        //cout << qPrintable(e.tagName()) << endl; // the node really is an element.
        //sleep(2);
      }
      if (e.tagName()=="name")
      {
        //cout << qPrintable(e.text()) << endl; // the node really is an element.
        track_list_->addItem(e.text());
      }


#endif
      //cout << qPrintable(e.tagName()) << endl; // the node really is an element.
      //sleep(2);

#if 0
      //if (e.tagName()=="rtept")
      if (e.tagName()==tag_name)
      {
        //qDebug() << e.attribute("lat");
        //qDebug() << e.attribute("lon");
	points_ += ("[" + e.attribute("lat") + "," + e.attribute("lon") + "],");
	//cout << "[" << qPrintable(e.attribute("lat")) << "," << qPrintable(e.attribute("lon")) << "]," << endl;

	//text_edit_->insertPlainText(e.attribute("lat"));
	//text_edit_->insertPlainText(e.attribute("lon"));
      }
#endif
         QDomNode node = e.firstChild();
         if(!node.isNull()) 
	 {
           //cout << "!node.isNull()" << endl;
           get_trk_info(node, tag_name);
	 }
	 else
	 {
           //cout << "node.isNull()" << endl;
           //return;
	 }
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

void MainWindow::open_file_slot()
{
  QDomNodeList nodes=dom_doc_.elementsByTagName("open_file_path");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.

  if (dirname_.isNull())
  {
    dirname_= e.attribute("path");
  }
  

  //file_name_ = QFileDialog::getOpenFileName(this, tr("Open GPX"), dirname_);
  fn_list_ = QFileDialog::getOpenFileNames(this, tr("Open GPX"), dirname_, "*.gpx");
  if (fn_list_.length()==0) return;

  color_index_ = 1;

  QObject::disconnect(files_, SIGNAL(currentIndexChanged ( int )), this, SLOT(select_gpx_file(int)));
  files_->clear();

  QString fn;

#if 0
  int add_file_trk_attr_count=fn_list_.length()-file_trk_attr_.size();

  for (int i=0 ; i < add_file_trk_attr_count ; ++i)
  { // add new FileTrkAttr* to file_trk_attr_
    file_trk_attr_.push_back(new FileTrkAttr);
  }
#endif

  // need free file_trk_attr_[0] ... file_trk_attr_[size-1] FileTrkAttr
  // then call clear() to clear all map element.
  // If don't do this, will get memory leak issue.
  map<QString, FileTrkAttr*>::iterator it;
  for (it = file_trk_attr_.begin() ; it != file_trk_attr_.end() ; ++it)
    delete it->second; 
  file_trk_attr_.clear();

  for (int i=0 ; i < fn_list_.length() ; ++i)
  {
    fn=fn_list_.at(i);
    file_trk_attr_[fn]=new FileTrkAttr;
    basename_=fn.right(fn.size()-fn.lastIndexOf('/')-1);
    files_->addItem(basename_);

    MapAttribute map_attr;
    map_attr.name=basename_;
    map_attr.color=i%(sizeof(colors)/sizeof(char*));
    map_attr_[i]=map_attr;
  }
  parse_gpx_.clear();
  dirname_=fn.left(fn.lastIndexOf("/"));
  QObject::connect(files_, SIGNAL(currentIndexChanged ( int )), this, SLOT(select_gpx_file(int)));

  get_trk(fn_list_.at(0), 0); // get the first file all trk name

  FileTrkAttr *first_file_trk_attr = 0;
#if 0
  if (file_trk_attr_.count(dirname_ + "/" + files_->itemText(0)))
    first_file_trk_attr = file_trk_attr_[(dirname_ + "/" + files_->itemText(0))];
#else
  if (file_trk_attr_.count(fn_list_.at(0)))
    first_file_trk_attr = file_trk_attr_[fn_list_.at(0)];
#endif

  if (first_file_trk_attr)
  {
    //qDebug() << "asd;" << (*first_file_trk_attr)[0].name;
    route_name_->setText((*first_file_trk_attr)[0].name);
  }



  text_edit_->clear();
  e.setAttribute("path", dirname_);
  statusBar()->showMessage(tr("open"));
  color_button_->setEnabled(true);
}

QString MainWindow::check_gpx_type(const QDomDocument &dom_doc)
{
  QDomNodeList node_list=dom_doc.elementsByTagName("rtept"); // m241 format

  if (node_list.size() == 0)
  {
    node_list=dom_doc.elementsByTagName("trkpt"); // general route format
    if (node_list.size() == 0)
    {
      return ""; // can not support the GPX format
    }
    else
    {
      return "trkpt";
    }
  }
  else
  {
    return "rtept";
  }
}

void MainWindow::search_all(QDomNode &n, const QString &tag_name)
{
  while(!n.isNull()) 
  {
    //cout << "aaa" << endl;
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull()) 
    {
      //cout << qPrintable(e.tagName()) << endl; // the node really is an element.
      //if (e.tagName()=="trkpt")
      //if (e.tagName()=="rtept")
      if (e.tagName()==tag_name)
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
           search_all(node, tag_name);
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

#if 0
1st format, no name attribute
<trk>
<trkseg>
<trkpt lat="25.000238000" lon="121.612724000">

2nd format, has name attribute
<trk>
<name>Track 101</name>
 <desc>Total Track Points: 765. Total time: 1h18m51s. Journey: 15.356Km</desc>
 <trkseg>

3rd format, has name attribute
<rte>
<name>Route 0</name>
<rtept lat="25.045307" lon="121.615578">




#endif

void MainWindow::get_trk_name(QDomNode &node, QString &trk_name)
{
  QDomNode n = node;

  while(!n.isNull()) 
  {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if (e.tagName()=="name")
    {
      trk_name=e.text();
      return;
    }
    if (e.tagName()=="trkseg")
    {
      trk_name=""; // not found trk name attribute.
      return;
    }
    n = n.firstChild();

  }
}

void MainWindow::get_trk_points(QDomNode &n, const QString &tag_name, QString &points)
{
  static bool a_trk_seg=false;
  //qDebug() << "tag_name: " << tag_name;
  while(!n.isNull()) 
  {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull()) 
    {
      //cout << qPrintable(e.tagName()) << endl; // the node really is an element.
      //sleep(1);
      if (e.tagName()=="trkseg")
      { // begin collect all points
        //qDebug() << "1"; 
        a_trk_seg=true;
      } // if (e.tagName()=="trkseg")
      if (e.tagName()=="/trkseg")
      { // begin collect all points
        //qDebug() << "2"; 
        a_trk_seg=false;
	return;
      } // if (e.tagName()=="trkseg")

      // rtept has no trkseg tag
      if (((a_trk_seg==true && e.tagName()==tag_name)) || e.tagName()=="rtept")
      {
        //qDebug() << "3"; 
	points += ("[" + e.attribute("lat") + "," + e.attribute("lon") + "],");
	//qDebug() << e.attribute("lat") + "," + e.attribute("lon");

	//text_edit_->insertPlainText(e.attribute("lon"));
      }
         QDomNode node = e.firstChild();
         if(!node.isNull()) 
	 {
           get_trk_points(node, tag_name, points);
	 }
	 else
	 {
           //cout << "node.isNull()" << endl;
           //return;
	 }
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

void MainWindow::create_html_file(QByteArray &template_data)
{
  QString write_trk;
  QList<QTreeWidgetItem *> select_items=select_route_view_->selectedItems();

  // preview all items
  if (select_items.count()==0)
  {
    QTreeWidgetItemIterator it(select_route_view_);
    while (*it) 
    {
      select_items.append(*it);
      ++it;
    }

  }

  qDebug() << "MainWindow::preview_without_save_slot";
  int t=1;
  for (int i=0; i < select_items.count() ; ++i)
  {
    RouteItem *ri = (RouteItem*)select_items.at(i);

      QString hc;

      MapAttribute trk_attr=ri->get_attr();
      qcolor2html_color_str(trk_attr.qc, hc);

      write_trk += QString("t = %1; trk_info[t] = []; \
    trk_info[t]['name'] = '%2'; trk_info[t]['desc'] = ''; trk_info[t]['clickable'] = true; \
    trk_info[t]['color'] = '%3'; trk_info[t]['width'] = 3; trk_info[t]['opacity'] = 0.8; \
    trk_info[t]['outline_color'] = '#000000'; trk_info[t]['outline_width'] = 0; trk_info[t]['fill_color'] = '#E60000'; trk_info[t]['fill_opacity'] = 0; \
    trk_segments[t] = [];").arg(t).arg(trk_attr.name).arg(hc);
	qDebug() << "trk_attr.points: " << trk_attr.points;

      write_trk += "trk_segments[t].push({points:[" + trk_attr.points + \
	            QString("]}); \
                \nGV_Draw_Track(t); \
	        t = %1; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});\n").arg(t);
      ++t;

  }



  //qDebug() << write_trk;

  QString template_fn="template.html";
  QFile template_file;
  template_file.setFileName(template_fn);
  if (!template_file.open(QIODevice::ReadOnly))
  {
    QMessageBox::warning(this, tr("gpx2map"),
                             tr("Cannot open file %1:\n%2.")
                             .arg(template_fn)
                             .arg(template_file.errorString()));

    return;
  }
  template_data=template_file.readAll();
  int pos=0;

  // replace ##, center point
  pos=template_data.indexOf("##");
  template_data.replace(pos, 2, center_point_.toAscii());

  // replace ^^
  pos=template_data.indexOf("^^");
  template_data.replace(pos, 2, google_map_key_->text().toAscii());

  // replace @@
  pos=0;
  pos=template_data.indexOf("@@");
  qDebug() << "pos: " << pos;
  //template_data.replace(pos, 2, points_.toAscii());
  template_data.replace(pos, 2, write_trk.toUtf8());
  template_file.close();
  //qDebug() << "template_data: " << template_data;
}

void MainWindow::preview_without_save_slot()
{
  QByteArray template_data;
  create_html_file(template_data);

  //qDebug() << "xx template_data: " << template_data;
  QFile temp_qf;
  preview_fn_ = QFSFileEngine::tempPath() + "t_view.html";

  temp_qf.setFileName(preview_fn_);
  if (!temp_qf.open(QIODevice::WriteOnly))
  {
    QMessageBox::warning(this, tr("gpx2map"),
                             tr("Cannot save temp file %1:\n%2.")
                             .arg(preview_fn_)
                             .arg(temp_qf.errorString()));

    return;
  }
  int w_len=temp_qf.write(template_data);
  qDebug() << "w_len: " << w_len;

#ifdef Q_OS_WIN32
  preview_fn_.prepend("file:///");
#else
  preview_fn_.prepend("file://");
#endif
  qDebug() << "preview_fn_ : " << preview_fn_;
  if (browser_==0)
    browser_ = new BrowserWindow(preview_fn_);
  else
  {
    browser_->load(preview_fn_);
  }
  browser_->show();

}

void MainWindow::save_as_slot()
{
  QDomNodeList nodes=dom_doc_.elementsByTagName("save_file_path");
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.

  if (save_dirname_.isNull())
  {
    save_dirname_= e.attribute("path");
  }

  backup_fn_= file_name_ + ".bak";
  file_name_ = QFileDialog::getSaveFileName(this, tr("Save As"), save_dirname_);
  if (file_name_.isNull()) return;
  save_dirname_=file_name_.left(file_name_.lastIndexOf("/"));
  basename_=file_name_.right(file_name_.size() - file_name_.lastIndexOf('/')-1);
  e.setAttribute("path", save_dirname_);

  save_file_slot();
}

// calculate center point, but is minus coordinate,
// the algorithm will calculate wrong center point.
void MainWindow::get_points(const QDomDocument &dom_doc)
{
  //QDomNodeList node_list=dom_doc.elementsByTagName("trkpt");
  //QDomNodeList node_list=dom_doc.elementsByTagName("rtept");
  //QString tag_name=check_gpx_type(dom_doc);

  QDomNodeList node_list=dom_doc.elementsByTagName(check_gpx_type(dom_doc));
  static double bx=0.0, by=0.0, sx=200.0, sy=200.0;
  QLocale c(QLocale::C);
  for (int i=0 ; i < node_list.size() ; ++i)
  {
    //qDebug() << "node_list.at(i): " << node_list.at(i);
    QDomElement e = node_list.at(i).toElement(); // try to convert the node to an element.
    //QString points = ("[" + e.attribute("lat") + "," + e.attribute("lon") + "],");
    double x = e.attribute("lat").toDouble();
    double y = e.attribute("lon").toDouble();
    {
      if (x>bx) bx=x;
      if (y>by) by=y;

      if (x<sx) sx=x;
      if (y<sy) sy=y;
    }
    //qDebug() << "points: " << points;
    //qDebug() << "bx: " << bx << " by: " << by << "by: " << sx << "sy: " << sy;


    //qDebug() << "x: " << x << " y: " << y ;
    //cout << "x: " << x << " y: " << y << endl;
  }
  qDebug() << "bx: " << c.toString(bx,'f',10) << " by: " << c.toString(by,'f',10) << "by: " << c.toString(sx,'f',10) << "sy: " << c.toString(sy,'f',10);

  center_point_= c.toString(((sx+bx)/2), 'f', 10) + ',' + c.toString(((sy+by)/2), 'f', 10) ;
}

int MainWindow::write_to_save_file(const QString &w_fn)
{
  QFile qf;
  QFile template_file;

  qDebug() << "w_fn: " << w_fn;
  // update FileTrkAttr
  load_gpx_attr(track_list_->currentIndex());

  // save map attribute, in the place should do the action,
  // because files_ doesn't single the currentIndexChanged(int), files_ will not recode 
  // the gpx attritube
  MapAttribute map_attr;
  map_attr.name=route_name_->text();
  map_attr_[previous_fn_index_]=map_attr;

  points_ = "";

  QString write_trk;

  int t=1; // for track index
  for (int i=0 ; i < fn_list_.length() ; ++i)
  {
    QString fn;
    fn=fn_list_.at(i);
    if (fn.isNull()) break;

    if (!parse_gpx_.count(fn))
    {
      //qDebug() << "xx";
      get_trk(fn_list_.at(i), i); 
    }
    else
    {
      //qDebug() << "yy";
    }

    qf_.setFileName(fn);
    if (!qf_.open(QIODevice::ReadOnly))
    {
      // ref : Qt-4.6.2/examples/xml/dombookmarks/mainwindow.cpp
      QMessageBox::warning(this, tr("gpx2map"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file_name_)
                             .arg(qf_.errorString()));
      break;
    }


    QDomDocument doc("mydocument");

    if (!doc.setContent(&qf_)) 
    {
      qf_.close();
      QMessageBox::warning(this, tr("gpx2map"), tr("%1 is not a gpx format file.").arg(file_name_));
      break;
    }
    qf_.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();
    //cout << "root tagname: " << qPrintable(docElem.tagName()) << endl; // the node really is an element.

    QDomNode n = docElem.firstChild();
    //FileTrkAttr* cur_file_trk_attr = file_trk_attr_[fn];

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
    
    FileTrkAttr* cur_file_trk_attr = 0;

    if (file_trk_attr_.count(fn))
      cur_file_trk_attr = file_trk_attr_[fn];

    for (int i=0 ; i < cur_file_trk_attr->size() ; ++i)
    {
      QString hc;

      MapAttribute trk_attr=(*cur_file_trk_attr)[i];
      qcolor2html_color_str((*cur_file_trk_attr)[i].qc, hc);

      write_trk += QString("t = %1; trk_info[t] = []; \
    trk_info[t]['name'] = '%2'; trk_info[t]['desc'] = ''; trk_info[t]['clickable'] = true; \
    trk_info[t]['color'] = '%3'; trk_info[t]['width'] = 3; trk_info[t]['opacity'] = 0.8; \
    trk_info[t]['outline_color'] = '#000000'; trk_info[t]['outline_width'] = 0; trk_info[t]['fill_color'] = '#E60000'; trk_info[t]['fill_opacity'] = 0; \
    trk_segments[t] = [];").arg(t).arg(trk_attr.name).arg(hc);
	//qDebug() << "trk_attr.points: " << trk_attr.points;

      write_trk += "trk_segments[t].push({points:[" + trk_attr.points + \
	            QString("]}); \
                \nGV_Draw_Track(t); \
	        t = %1; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});\n").arg(t);
      ++t;

    }


    get_points(doc);

    //search_all(n, check_gpx_type(doc));

#if 0
    t = 1; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});
    t = 2; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});

#endif
    //cout << "]" << endl;

  }

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

  // replace ##, center point
  pos=template_data.indexOf("##");
  template_data.replace(pos, 2, center_point_.toAscii());

  // replace ^^
  pos=template_data.indexOf("^^");
  template_data.replace(pos, 2, google_map_key_->text().toAscii());

  // replace @@
  pos=0;
  pos=template_data.indexOf("@@");
  qDebug() << "pos: " << pos;
  //template_data.replace(pos, 2, points_.toAscii());
  template_data.replace(pos, 2, write_trk.toUtf8());
  template_file.close();

  text_edit_->insertPlainText(template_data);

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

#if 0
  QMessageBox::warning(this, tr("gpx2map"),
                             tr("write save file %1:\n.")
                             .arg(w_fn));
#endif

  qf.close();
  preview_fn_ = w_fn;
  return 0;
}

void MainWindow::save_file_slot()
{
  if (backup_fn_.isEmpty())
    backup_fn_= file_name_ + ".bak";

  // wirte to file
  write_to_save_file(file_name_);

}

void MainWindow::change_save_file_offset ( int index )
{

}

void MainWindow::select_gpx_file(int index)
{
  get_trk(dirname_ + "/" + files_->itemText(index), index); // get the first file all trk name
  formGroupBox->setTitle(files_->currentText());
}

void MainWindow::load_gpx_attr(int index)
{
  //qDebug() << "index: " << index;
  //qDebug() << "files_->currentIndex(): " << files_->currentIndex();
  //qDebug() << "file_trk_attr_.size(): " << file_trk_attr_.size();
  if (index < 0)
    return;

  FileTrkAttr* cur_file_trk_attr = file_trk_attr_[dirname_ + "/" + files_->currentText()];
  //qDebug() << "cur_file_trk_attr->size(): " << cur_file_trk_attr->size();
  //qDebug() << "cur_file_trk_attr:" << cur_file_trk_attr;

  //get_trk(dirname_ + "/" + files_->itemText(index), index); // get the first file all trk name

  //MapAttribute map_attr;


#if 0
  // save map attribute
  //map_attr.name=route_name_->text();
  (*cur_file_trk_attr)[index].name = route_name_->text();

  (*cur_file_trk_attr)[index].color = color_combobox_->currentIndex();
#else

  //qDebug() << (*cur_file_trk_attr)[index].name;
  MapAttribute trk_attr = cur_file_trk_attr->at(index);

  //qDebug() << "(*cur_file_trk_attr)[index].name:" << (*cur_file_trk_attr)[index].name;
  (*cur_file_trk_attr)[previous_fn_index_].name = route_name_->text();
  route_name_->setText((*cur_file_trk_attr)[index].name);
#endif
  //map_attr.color=color_combobox_->currentText();
  //map_attr_[previous_fn_index_]=map_attr;

#if 0
  // load current index map attribute
  if (map_attr_.count(index))
  {
    route_name_->setText(map_attr_[index].name);
    color_combobox_->setCurrentIndex(map_attr_[index].color);
    //color_combobox_->setItemText(previous_fn_index_, map_attr_[index].color);
  }
  else
  { // not yet set map attribute
    route_name_->setText("");
    color_combobox_->setCurrentIndex(0);
    //color_combobox_->setCurrentText("#800080");
    //color_combobox_->setItemText(0, "#E60000");
  }
#endif

  previous_fn_index_ = index;

#if 0
  if (index > map_attr_.length())
  { // add new map_attr

  }
  else
  { // edit old map_attr

  }
  MapAttribute map_attr;
#endif

}

// copy from: 
//      /usr/local/Trolltech/Qt-4.6.2/examples/layouts/basiclayouts/dialog.cpp
void MainWindow::create_form_groupbox()
{
  //QBoxLayout *line_layout = new QBoxLayout(QBoxLayout::LeftToRight);
  //QBoxLayout *line_layout = new QBoxLayout(QBoxLayout::TopToBottom);
  //QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);

  QDomNodeList nodes=dom_doc_.elementsByTagName("google_map_key");
  qDebug() << "nodes.size() : " << nodes.size();
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.
  qDebug() << "e.attribute(k1): " << e.attribute("k1");
  google_map_key_ = new QLineEdit(e.attribute("k1"), this);

  
  formGroupBox = new QGroupBox(tr("Select GPX Files"));

  formGroupBox->setLayout(layout);

  text_edit_ = new QTextEdit(0);
  //layout->addWidget(text_edit_);

  route_view_ = new QTreeWidget(this);
  layout->addWidget(route_view_);
  QStringList labels;
  labels << tr("origianl route name") << tr("modified route name") << tr("route color") << tr("file");
  route_view_->setHeaderLabels(labels);
  route_view_->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QBoxLayout *v_layout = new QBoxLayout(QBoxLayout::TopToBottom);

  layout->addLayout(v_layout);

  rv_add_ = new QPushButton(tr("add gpx files"));
  v_layout->addWidget(rv_add_);

  rv_remove_all_ = new QPushButton(tr("remove all"));
  v_layout->addWidget(rv_remove_all_);

  rv_remove_ = new QPushButton(tr("remove select route"));
  v_layout->addWidget(rv_remove_);

  select_route_button_ = new QPushButton(tr("select routes"));
  v_layout->addWidget(select_route_button_);


  //QTreeWidgetItem *item = new QTreeWidgetItem(route_view_);
  //item->setText(0, "route 1");
  //item->setText(1, "route color 1");
  select_route_view_ = new QTreeWidget(this);
  //select_route_view_->setSelectionMode(QAbstractItemView::MultiSelection);
  select_route_view_->setHeaderLabels(labels);
  select_route_view_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  layout->addWidget(select_route_view_);

  QBoxLayout *v_layout1 = new QBoxLayout(QBoxLayout::TopToBottom);

  layout->addLayout(v_layout1);

  v_layout1->addWidget(new QLabel(tr("google map key")));
  v_layout1->addWidget(google_map_key_);

  sel_rv_remove_all_ = new QPushButton(tr("remove all"));
  v_layout1->addWidget(sel_rv_remove_all_);

  sel_rv_remove_ = new QPushButton(tr("remove select route"));
  v_layout1->addWidget(sel_rv_remove_);

  preview_button_ = new QPushButton(tr("preview"));
  v_layout1->addWidget(preview_button_);

  rv_save_to_html_ = new QPushButton(tr("save to html"));
  v_layout1->addWidget(rv_save_to_html_);


  QObject::connect(select_route_button_, SIGNAL(pressed( )), this, SLOT(select_route_slot()));
  QObject::connect(preview_button_, SIGNAL(pressed( )), this, SLOT(preview_without_save_slot()));

  QObject::connect(select_route_view_, SIGNAL(itemClicked ( QTreeWidgetItem * , int ) ), this, SLOT(open_color_dialog(QTreeWidgetItem * , int ) ));
  QObject::connect(select_route_view_, SIGNAL(itemChanged ( QTreeWidgetItem * , int ) ), this, SLOT(modify_route_name_slot(QTreeWidgetItem * , int ) ));

  QObject::connect(rv_remove_all_, SIGNAL(pressed( )), this, SLOT(rv_remove_all_slot()));
  QObject::connect(sel_rv_remove_all_, SIGNAL(pressed( )), this, SLOT(sel_rv_remove_all_slot()));

  QObject::connect(rv_remove_, SIGNAL(pressed( )), this, SLOT(rv_remove_slot()));
  QObject::connect(sel_rv_remove_, SIGNAL(pressed( )), this, SLOT(sel_rv_remove_slot()));
  QObject::connect(rv_add_, SIGNAL(pressed( )), this, SLOT(open_file_slot()));

  QObject::connect(rv_save_to_html_, SIGNAL(pressed( )), this, SLOT(rv_save_to_html_slot()));

}

void MainWindow::rv_save_to_html_slot()
{
  QFile temp_qf;
  file_name_ = QFileDialog::getSaveFileName(this, tr("Save As"), save_dirname_);
  if (file_name_=="") return;

  QByteArray template_data;
  create_html_file(template_data);

  temp_qf.setFileName(file_name_);
  if (!temp_qf.open(QIODevice::WriteOnly))
  {
    QMessageBox::warning(this, tr("gpx2map"),
                             tr("Cannot save html file %1:\n%2.")
                             .arg(file_name_)
                             .arg(temp_qf.errorString()));

    return;
  }
  int w_len=temp_qf.write(template_data);

}

void MainWindow::rv_remove_slot()
{
  //route_view_->clear();
  QList<QTreeWidgetItem *> select_items=route_view_->selectedItems();
  for (int i=0; i < select_items.count() ; ++i)
  {
    RouteItem *ri = (RouteItem*)select_items.at(i);
    delete ri;
  }

}

void MainWindow::sel_rv_remove_slot()
{
  //select_route_view_->takeTopLevelItem(0);
  QList<QTreeWidgetItem *> select_items=select_route_view_->selectedItems();
  for (int i=0; i < select_items.count() ; ++i)
  {
    RouteItem *ri = (RouteItem*)select_items.at(i);
    delete ri;
  }
}

void MainWindow::rv_remove_all_slot()
{
  route_view_->clear();
}

void MainWindow::sel_rv_remove_all_slot()
{
  select_route_view_->clear();
}

void MainWindow::select_route_slot()
{
  qDebug() << "select route";
  QList<QTreeWidgetItem *> select_items=route_view_->selectedItems();
  for (int i=0; i < select_items.count() ; ++i)
  {
    RouteItem *ri = (RouteItem*)select_items.at(i);
    RouteItem *item = new RouteItem(select_route_view_);
    item->setText(0, select_items.at(i)->text(0));
    item->setText(1, select_items.at(i)->text(1));
    item->set_attr(ri->get_attr());
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->update_text();
  }
  //select_route_view_->insertTopLevelItems(1, select_items);

  //select_route_view_->addTopLevelItem(item);


  //select_route_view_ = new QTreeWidget(this);

}

void MainWindow::open_color_dialog(QTreeWidgetItem * item, int column)
{
  if (column != 2) return;

  RouteItem *ri=0;
  if (item)
    ri = (RouteItem*)item;

  QColor qc = QColorDialog::getColor(ri->get_qc(), this);
  if (qc.isValid())
  {
    ri->set_qc(qc);
    QString hc;
    qcolor2html_color_str(qc, hc);
    ri->setText(2, hc);
    ri->setForeground(2, qc);
    qDebug() << "hc:" << hc;
    //(*file_trk_attr)[track_list_->currentIndex()].qc=qc;
    //color_button_->setPalette(qc);
    //color_button_->setBackgroundRole(qc);
  }
}
void MainWindow::modify_route_name_slot(QTreeWidgetItem * item, int column)
{
  //qDebug() << "column:" << column;
  // update route name
  if (column == 1)
  {
    RouteItem *ri=0;
    if (item)
      ri = (RouteItem*)item;
    ri->set_rn(ri->text(1));
  }

}

void MainWindow::open_color_dialog()
{
  FileTrkAttr* file_trk_attr=get_cur_file_trk_attr();
  QColor qc = QColorDialog::getColor((*file_trk_attr)[track_list_->currentIndex()].qc, this);
  if (qc.isValid())
  {
    QString hc;
    qcolor2html_color_str(qc, hc);
    qDebug() << "hc:" << hc;
    (*file_trk_attr)[track_list_->currentIndex()].qc=qc;
    //color_button_->setPalette(qc);
    //color_button_->setBackgroundRole(qc);
  }


}

void MainWindow::show_debug_log_slot()
{
  text_edit_->show();
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

