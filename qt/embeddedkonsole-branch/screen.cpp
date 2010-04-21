#include "screen.h"
#include "MyPty.h"
#include "TEWidget.h"

int DS::Screen::add_char(const char *buf, int len)
{
  for (int i=0 ; i < len ; ++i)
  {
    if (buf[i] == '\n' || cur_x_ >= col_) 
    {
      ++cur_y_;
      cur_x_=0;
    }
    if (cur_y_ >= row_)
    {
      // move the oldest row to scroll_data
      move_row_to_scroll_data();
      //qDebug("move_row_to_scroll_data()");
      cur_x_=0;
      --cur_y_;
    }
    screen_data_.push_back(ca(buf[i]));
    printf("%c", buf[i]);

  }
  printf("\n");
  gui_->repaint();
  return 0;
}

int DS::Screen::get_pty_data(const char *buf, int len)
{
  if (te_) // parse buf terminal escape sequence
  {
  }
  add_char(buf, len);

  // add buf to screen_data
  

  return 0;
}

int DS::Screen::move_row_to_scroll_data()
{
  int move_count=0;

  deque<ca> :: iterator it=screen_data_.begin();
  for (int i=0 ; i < col_ ; ++i)
  {
    ++move_count;

    ca c=*it;
    if (c.c=='\n') break;

    scroll_data_.push_back(*it++);
  }

  qDebug("move_count: %d", move_count);
  for (int i=0 ; i < move_count ; ++i)
    screen_data_.pop_front();


  return 0;
}

int DS::Screen::key_press(int k)
{
  qDebug("k: %x", k);
  char c[2];
  c[0]=k;
  c[1]=0;
  mypty_->send_bytes(c,1);
  return 0;
}
