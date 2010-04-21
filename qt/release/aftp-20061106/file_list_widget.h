/*
 * This class to recode filename item
 *
 */
#include <QListWidgetItem>
#include <QListWidget>


#include <QTreeWidgetItemIterator>
#include <QTreeWidgetItem>
#include <QTreeWidget>


using namespace Qt;

class FileWidget: public QTreeWidget
{
  Q_OBJECT
  public:
    FileWidget(QWidget * parent = 0):QTreeWidget(parent){}
};

class FileListWidgetItem : public QTreeWidgetItem
{
  //Q_OBJECT
  public:
    FileListWidgetItem( QTreeWidget * parent, int type = Type ):QTreeWidgetItem(parent, type){}
    FileListWidgetItem( QTreeWidgetItem * parent, int type = Type ):QTreeWidgetItem(parent, type){}
    bool operator< ( const QTreeWidgetItem & other ) const;
    void set_real_str(const QString &str){real_str_=str;}
    QString get_real_str() const {return real_str_;}

#if 0
    FileListWidgetItem( const QString & text, QListWidget * parent = 0, int type = Type ){}
    FileListWidgetItem ( const QIcon & icon, const QString & text, QListWidget * parent = 0, int type = Type ){}
    FileListWidgetItem( const QListWidgetItem & other ){}
#endif
    bool is_dir() const {return is_dir_;}
    void set_dir(bool t){is_dir_=t;}
  private:
    bool is_dir_;
    QString real_str_; // recode non encoding ascii code for file name.
    int encode_;
    
};

class DirTreeView : public QTreeWidget
{
  Q_OBJECT
  public:
    DirTreeView( QWidget * parent = 0);
    int get_select_path(QString &path);
  public slots:
    void expend_dirs( QTreeWidgetItem * item, int column ) ;
  private:
};
