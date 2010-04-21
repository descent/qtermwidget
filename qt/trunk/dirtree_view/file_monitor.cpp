#include "file_monitor.h"

#include "dirtree_view.h"

const char *event_name(int code)
{
  static const char *famevent[] = 
  {
    "",
    "FAMChanged",
    "FAMDeleted",
    "FAMStartExecuting",
    "FAMStopExecuting",
    "FAMCreated",
    "FAMMoved",
    "FAMAcknowledge",
    "FAMExists",
    "FAMEndExist"
  };
  static char unknown_event[10];
		 
  if (code < FAMChanged || code > FAMEndExist)
  {
    sprintf(unknown_event, "unknown (%d)", code);
    return unknown_event;
  }
  return famevent[code];
}

FileMonitor::FileMonitor()
{
  //fam_=0;
  fam_notifier_=0;
  error_=0;
}

FileMonitor::~FileMonitor()
{
  if (error_!=0) return;
  close_fam();
}

int FileMonitor::open_fam()
{
  if (FAMOpen(&fam_) != 0)
  {
    perror("fam");
    error_=-1;
    return -1;
  }
  int fam_fd=FAMCONNECTION_GETFD(&fam_);
  int flags=fcntl(fam_fd, F_GETFL);
  if (!(flags & O_NONBLOCK))
    fcntl(fam_fd, F_SETFL, flags|O_NONBLOCK);

  fam_notifier_=new QSocketNotifier(fam_fd, QSocketNotifier::Read, this);

  connect(fam_notifier_, SIGNAL(activated(int)), SLOT(check_fam() ));

  // this is in order to initialize fam_request_
  rc_=FAMMonitorDirectory(&fam_, "/", &fam_request_,NULL);
  if (rc_ != 0)
  {
    perror("fam: monitor dir");
    error_=-1;
    return rc_;
  }

  return 0;
}



int FileMonitor::close_fam()
{
  if (error_!=0) return -1;
  if (FAMClose(&fam_)!=0)
  {
    perror("fam close");
    error_=-1;
  }
  return 0;
}

int FileMonitor::check_fam()
{
  if (error_!=0) return -1;
  cout << "FileMonitor::check_fam()" << endl;
  while(FAMPending(&fam_))
  {
    FAMEvent ev;
    if(FAMNextEvent(&fam_, &ev) != 1)
    {
      close_fam();
      return -1;
    }

    //cout << event_name(ev.code) << endl;
    //cout << ev.filename << endl;

    emit file_alt(&ev);
  }
  return 0;
}

int FileMonitor::monitor_file(const char *fp)
{
  if (error_!=0) return -1;
  cout << "monitor : " << fp << endl;
  // when I moniotr another file, I need canel monitor first
  rc_=FAMCancelMonitor(&fam_,&fam_request_);
  if (rc_ != 0)
  {
    perror("fam: cancel monitor dir");
    error_=-1;
    return rc_;
  }
  rc_=FAMResumeMonitor(&fam_,&fam_request_);
  if (rc_ != 0)
  {
    perror("fam: resume monitor dir");
    error_=-1;
    return rc_;
  }

  rc_=FAMMonitorDirectory(&fam_, fp, &fam_request_,NULL);
  if (rc_ != 0)
  {
    perror("fam: monitor dir");
    error_=-1;
    return rc_;
  }
  return 0;

}
