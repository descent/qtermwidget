#ifndef FILE_MONITOR_H
#define FILE_MONITOR_H

#include <qobject.h>
#include <qsocketnotifier.h>

#include <unistd.h>
#include <fcntl.h>

#include <fam.h>

#include <iostream>

using namespace std;

const char *event_name(int code);

class FileMonitor : public QObject
{
  Q_OBJECT
  public:
    FileMonitor();
    ~FileMonitor();
    int open_fam();
    int close_fam();
    int monitor_file(const char* fp);
  signals:
    void file_alt(int code);
    void file_alt(FAMEvent *ev);
  private slots:
    int check_fam();
  private:
    FAMConnection fam_;
    FAMRequest fam_request_;
    QSocketNotifier *fam_notifier_;
    int error_;
    int rc_;

};

#endif
