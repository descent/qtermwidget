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

MainWindow::MainWindow():QMainWindow(), previous_fn_index_(0)
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
  file_menu_ = menuBar()->addMenu(tr("&File"));
  ADD_ACTION(file_menu_, open_file_, "&Open File", open_file_slot )
  //ADD_ACTION(file_menu_, save_file_, "&Save", save_file_slot )
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
  //msg+=" version";

   // ref : Qt-4.6.2/examples/xml/dombookmarks/mainwindow.cpp
   QMessageBox::about(this, tr("About GPX2MAP"), msg);

}


#define QT_FILE_IO

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

  FileTrkAttr* cur_file_trk_attr = file_trk_attr_[fn];

  cur_file_trk_attr->clear();
  for (int i=0 ; i < node_list.size() ; ++i)
  {
    QDomNode n = node_list.at(i).firstChild();
    //QDomNode node = e.firstChild();

    if(!n.isNull()) 
    {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if(!e.isNull()) 
      {
        //cout << qPrintable(e.tagName()) << endl; // the node really is an element.
        track_list_->addItem(e.text());
	MapAttribute trk_attr;
        trk_attr.name=e.text();
	//qDebug() << e.text();
        trk_attr.color=i%(sizeof(colors)/sizeof(char*));

	// current code, points will do many copy time.
        get_trk_points(n, tag_name, trk_attr.points);
        cur_file_trk_attr->push_back(trk_attr);
      }
    }
    //get_trk_points(n.firstChild(), tag_name);
  }

  route_name_->setText((*cur_file_trk_attr)[0].name);
  color_combobox_->setCurrentIndex((*cur_file_trk_attr)[0].color);
  QObject::connect(track_list_, SIGNAL(currentIndexChanged ( int )), this, SLOT(load_gpx_attr(int)));
  unsetCursor();

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

  files_->clear();

  QString fn;

#if 0
  int add_file_trk_attr_count=fn_list_.length()-file_trk_attr_.size();

  for (int i=0 ; i < add_file_trk_attr_count ; ++i)
  { // add new FileTrkAttr* to file_trk_attr_
    file_trk_attr_.push_back(new FileTrkAttr);
  }
#endif

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

  get_trk(fn_list_.at(0), 0); // get the first file all trk name


  dirname_=fn.left(fn.lastIndexOf("/"));

  text_edit_->clear();
  e.setAttribute("path", dirname_);
  statusBar()->showMessage(tr("open"));
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

      if (a_trk_seg==true && e.tagName()==tag_name)
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


void MainWindow::save_as_slot()
{
  backup_fn_= file_name_ + ".bak";
  file_name_ = QFileDialog::getSaveFileName(this, tr("Open Rich8 to Save As"), dirname_);
  if (file_name_.isNull()) return;
  dirname_=file_name_.left(file_name_.lastIndexOf("/"));
  basename_=file_name_.right(file_name_.size() - file_name_.lastIndexOf('/')-1);

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

  // save map attribute, in the place should do the action,
  // because files_ doesn't single the currentIndexChanged(int), files_ will not recode 
  // the gpx attritube
  MapAttribute map_attr;
  map_attr.name=route_name_->text();
  map_attr.color=color_combobox_->currentIndex();
  //map_attr.color=color_combobox_->currentText();
  map_attr_[previous_fn_index_]=map_attr;

  points_ = "";

  for (int i=0 ; i < fn_list_.length() ; ++i)
  {
    QString fn;
    fn=fn_list_.at(i);
    if (fn.isNull()) break;

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
    points_ += QString("t = %1; trk_info[t] = []; \
    trk_info[t]['name'] = '%2'; trk_info[t]['desc'] = ''; trk_info[t]['clickable'] = true; \
    trk_info[t]['color'] = '%3'; trk_info[t]['width'] = 3; trk_info[t]['opacity'] = 0.8; \
    trk_info[t]['outline_color'] = '#000000'; trk_info[t]['outline_width'] = 0; trk_info[t]['fill_color'] = '#E60000'; trk_info[t]['fill_opacity'] = 0; \
    trk_segments[t] = [];").arg(i+1).arg(map_attr_[i].name).arg(colors[map_attr_[i].color]);

    points_ += "trk_segments[t].push({points:[";

    get_points(doc);

    search_all(n, check_gpx_type(doc));
    points_ += QString("]}); \
                \nGV_Draw_Track(t); \
	        t = %1; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});\n").arg(i+1);
#if 0
    t = 1; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});
    t = 2; GV_Add_Track_to_Tracklist({bullet:'- ',name:trk_info[t]['name'],desc:trk_info[t]['desc'],color:trk_info[t]['color'],number:t});

#endif
    //cout << "]" << endl;

  }





  //text_edit_->insertPlainText(points_);





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
  template_data.replace(pos, 2, points_.toAscii());
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

  //map_attr.color=color_combobox_->currentIndex();
  (*cur_file_trk_attr)[index].color = color_combobox_->currentIndex();
#else

  //qDebug() << (*cur_file_trk_attr)[index].name;
  MapAttribute trk_attr = cur_file_trk_attr->at(index);

  //qDebug() << "(*cur_file_trk_attr)[index].name:" << (*cur_file_trk_attr)[index].name;
  route_name_->setText((*cur_file_trk_attr)[index].name);
  color_combobox_->setCurrentIndex((*cur_file_trk_attr)[index].color);
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
  QFormLayout *form_layout = new QFormLayout;

  color_combobox_ = new QComboBox(this);
  files_ = new QComboBox(this);
  track_list_ = new QComboBox(this);
  track_list_->setEditable(true);
  route_name_ = new QLineEdit(tr(""), this);
  //google_map_key_ = new QLineEdit("ABQIAAAA8FCDZv0GdTV1ZXaxaBQ9pBTwGRZDfZiPh3bZ0KEOkhpQKe-QJxRFj7qYGmmzROwQb02-A0lCig73Fg", this);
  QDomNodeList nodes=dom_doc_.elementsByTagName("google_map_key");
  qDebug() << "nodes.size() : " << nodes.size();
  QDomElement e = nodes.at(0).toElement(); // try to convert the node to an element.
  qDebug() << "e.attribute(k1): " << e.attribute("k1");
  google_map_key_ = new QLineEdit(e.attribute("k1"), this);

  form_layout->addRow(new QLabel(tr("file list")), files_);
  form_layout->addRow(new QLabel(tr("track list")), track_list_);

  form_layout->addRow(new QLabel(tr("route name")), route_name_);
  form_layout->addRow(new QLabel(tr("route color")), color_combobox_);
  form_layout->addRow(new QLabel(tr("google map key")), google_map_key_);
  
#if 0
  line_layout->addWidget(new QLabel(tr("File List: ")));
  line_layout->addWidget(files_);

  line_layout->addWidget(new QLabel(tr("Route Name: ")));
  line_layout->addWidget(route_name_);
  line_layout->addWidget(new QLabel(tr("Color: ")));
  line_layout->addWidget(color_combobox_);
#endif

  for (int i=0 ; i < sizeof(colors)/sizeof(char*) ; ++i)
  {
    color_combobox_->addItem(colors[i]);
  }
    //QString styleSheet = "QComboBox   {  color:  #0000A0;   }";
    //color_combobox_->setStyleSheet(styleSheet);
  #if 0
  QAbstractItemView *v = color_combobox_->view();
  for (int i=0 ; i < sizeof(colors)/sizeof(char*) ; ++i)
  {
    QWidget* w=v->indexWidget(i);
  }
  #endif

  //layout->addLayout(line_layout);
  layout->addLayout(form_layout);


  formGroupBox = new QGroupBox(tr("Select GPX Files"));

  formGroupBox->setLayout(layout);

  text_edit_ = new QTextEdit(this);
  layout->addWidget(text_edit_);

  QObject::connect(files_, SIGNAL(currentIndexChanged ( int )), this, SLOT(select_gpx_file(int)));
  QObject::connect(track_list_, SIGNAL(currentIndexChanged ( int )), this, SLOT(load_gpx_attr(int)));
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

