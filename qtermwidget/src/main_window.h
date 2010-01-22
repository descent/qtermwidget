#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
//#include <QAction>
//#include <QMenu>
//#include <QMenu>

#if 1
//QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QTextEdit;
class QTabWidget;
class QTermWidget;

#include <QTabWidget>

//QT_END_NAMESPACE
#endif

#if 0
class TerminalTabWidget : public QTabWidget
{
  Q_OBJECT
  public:

    TerminalTabWidget(QWidget * parent = 0 ):QTabWidget(parent)
    {
    }
  protected:
    virtual void keyPressEvent(QKeyEvent* event);
  
};
#endif

//! [0]
class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow();
    QTermWidget *create_qterm_widget();
    void close_tab(int tab_index);



  private:
    QTabWidget *tab_widget_;

    QMenu *setting_menu_;
    QMenu *file_menu_, *edit_menu_;
    QAction *new_tab_, *close_tab_;
    QAction *copy_, *paste_;
    QAction *change_font_;
    QAction *change_bg_color_;
    QAction *set_encode_;
    QAction *big5_enc_, *utf8_enc_;
    QMenu *encoding_menu_;
    QActionGroup *input_enc_g_;
    QActionGroup *enc_g_;

  public slots:
    void switch_tab_slot(int tab_index);
    void new_tab_slot();
    void close_tab_slot();
    void big5_enc();
    void utf8_enc();
    void copy_slot();
    void paste_slot();
    bool close();
  private slots:
    void change_font_slot();
    void change_bg_color_slot();
    void set_encode_slot();
#if 0
    //virtual ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    //void change_font_slot();
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    //void change_font_slot();
    void documentWasModified();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QTextEdit *textEdit;
    QString curFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    //QMenu *setting_menu_;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    //QAction *change_font_;
#endif
};
//! [0]

#endif
