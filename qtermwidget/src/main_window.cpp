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

#include "main_window.h"
#include "qtermwidget.h"



#include <cstdio>

using namespace std;

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


MainWindow::MainWindow():QMainWindow()
{

  //QList<QKeySequence> key_shortcuts;

  file_menu_ = menuBar()->addMenu(tr("&File"));
  ADD_ACTION(file_menu_, new_tab_, "&new tab", new_tab_slot )
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
  setting_menu_ = menuBar()->addMenu(tr("&Setting"));

  ADD_ACTION(setting_menu_, change_bg_color_, "&BackgroundColor", change_bg_color_slot);
  ADD_ACTION(setting_menu_, change_font_, "&Font", change_font_slot);

  SET_KEY_SHORTCUT(tr("Ctrl+B"), change_bg_color_);
  SET_KEY_SHORTCUT(tr("Ctrl+F"), change_font_);

  encoding_menu_ = menuBar()->addMenu(tr("E&ncoding") );

  ADD_ACTION(encoding_menu_, big5_enc_, "BIG5", big5_enc)
  ADD_ACTION(encoding_menu_, utf8_enc_, "UTF8", utf8_enc)

  help_menu_ = menuBar()->addMenu(tr("&Help"));
  ADD_ACTION(help_menu_, about_, "&About", about_slot)

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
}

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

void MainWindow::new_tab_slot()
{
  char tab_index[6];

  sprintf(tab_index, "%d", tab_widget_->count()+1);
  tab_widget_->addTab(create_qterm_widget(), QString(tab_index));
  utf8_enc();
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

const char *win_version_str()
{
#if 0
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
    default:
    {
      return "unknow";
    }
  }
  #endif
}

void MainWindow::about_slot()
{
  QMessageBox msg_box;
  QString msg;
#if defined(Q_CYGWIN_WIN)
  msg.sprintf("windows/cygwin\nQT version: %s", qVersion());
#elif defined(Q_WS_WIN) 
  QSysInfo::windowsVersion()  ;
  msg.sprintf("windows: %s\nQT version: %s", win_version_str(), qVersion());
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
  msg_box.exec();
  //qDebug("about");  	
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


#if 1
  static QFont terminal_font;
  terminal_font = QFontDialog::getFont(&ok, terminal_font ,this);
  //QFont terminal_font = QFontDialog::getFont(&ok, QFont("Monospace", 14) ,this);

  if (ok)
  {
    //terminal_font.setStyle(QFont::StyleNormal);
    //terminal_font.setFamily("Monospace");
    qDebug() << "family: " << terminal_font.family() ;
    if (terminal_font.pixelSize()==-1)
      qDebug() << " point size: " << terminal_font.pointSize() << endl;
    else
      qDebug() << " pixel size: " << terminal_font.pixelSize() << endl;

    for (int i=0 ; i < tab_widget_->count() ; ++i) 
    {
      (dynamic_cast<QTermWidget *> (tab_widget_->widget(i)))->setTerminalFont(terminal_font);
      (dynamic_cast<QTermWidget *> (tab_widget_->widget(i)))->get_terminal_display()->updateImage();
    }
    //if (tab_widget_->currentWidget())
      //(dynamic_cast<QTermWidget *> (tab_widget_->currentWidget()))->setTerminalFont(terminal_font);
    //(dynamic_cast<QTermWidget *> (tab_widget_->currentWidget()))->get_terminal_display()->updateImage();
    //qDebug("ok");
#if 0
    QMessageBox msg_box;
    msg_box.setText(terminal_font.family());
    msg_box.exec();
#endif


    
    //qDebug("font family: %s\n", terminal_font.family().toStdString().c_str());
  }
  else
  {
    qDebug("not ok");
  }
#endif

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
