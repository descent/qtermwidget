#ifndef FTP_BOOKMARK_h
#define FTP_BOOKMARK_h

#include <QFrame>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QFile>

#include <fstream>

using namespace std;

class QLabel;
class QTreeWidget;
class QString;
class QPushButton;

class FtpSiteItem : public QTreeWidgetItem
{
  public:
    FtpSiteItem(QTreeWidget *parent);
    void set_ftp_addr(const QString &str)
    {
      ftp_addr_ = str;
    }
    void set_ftp_port(const QString &str)
    {
      ftp_port_ = str;
    }
    void set_ftp_username(const QString &str)
    {
      ftp_username_ = str;
    }
    void set_ftp_password(const QString &str)
    {
      ftp_password_ = str;
    }

    const QString &get_ftp_addr() const
    {
      return ftp_addr_;
    }
    const QString & get_ftp_port() const
    {
      return ftp_port_;
    }
    const QString & get_ftp_username() const
    {
      return ftp_username_;
    }
    const QString & get_ftp_password() const
    {
      return ftp_password_;
    }
  private:
    QString ftp_addr_, ftp_port_, ftp_username_, ftp_password_;
};

#define GEN_SET_LINE_EDIT(attr) \
void set_ftp_##attr##_le(const QString &str) \
{ \
  ftp_##attr##_line_edit_ ->setText(str); \
}


class FtpBookmark : public QFrame
{
  Q_OBJECT
  public:
    FtpBookmark(QWidget * parent = 0, Qt::WindowFlags f = 0); 
    ~FtpBookmark();
    int write_bookmark_to_file();

    GEN_SET_LINE_EDIT(addr)
    GEN_SET_LINE_EDIT(port)
    GEN_SET_LINE_EDIT(username)
    GEN_SET_LINE_EDIT(password)

  public slots:
    void s_show_info( QTreeWidgetItem * item, int column );
    void s_add_bookmark();
    void s_connect();
    void s_edit_bookmark();
    void s_del_bookmark();
  signals:
    void connect_to_site(const FtpSiteItem *item);
  private:
    QLabel *ftp_site_label_,*ftp_addr_, *ftp_port_, *ftp_username_, *ftp_password_;
    QLineEdit *ftp_site_line_edit_, *ftp_addr_line_edit_, *ftp_port_line_edit_, *ftp_username_line_edit_, *ftp_password_line_edit_;
    QTreeWidget *ftp_site_;
    QPushButton *add_button_, *edit_button_, *del_button_, *connect_button_;
    //QFile ftp_bookmark_;
    fstream ftp_bookmark_;

};



#endif
