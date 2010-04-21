// this class present a terminal screen
//
#ifndef SCREEN_H
#define SCREEN_H

#include "TECommon.h"

#include <vector>
#include <deque>

using namespace std;

class TEWidget;
class MyPty;

namespace DS
{
  class TEmulationPasrer;

  class Screen
  {
    //typedef vector<ca> Line;
    public:
      Screen(MyPty *mypty, DS::TEmulationPasrer *te=0, int r=0, int c=0)
      {
        mypty_=mypty;
        screen_begin_=0;
        cur_x_=cur_y_=0;
        row_=r;
        col_=c;
	te_=te;
      }
      ~Screen(){}
      int change_size(int r, int c)
      {
        row_=r;
        col_=c;

	return 0;
      }
      // call by pty
      int get_pty_data(const char *buf, int len);
      int add_char(const char *buf, int len);
      void set_gui(TEWidget *gui){gui_=gui;}
      const deque<ca> &get_screen_data() {return screen_data_;}
      int key_press(int k);
    private:

      int move_row_to_scroll_data();

      TEWidget *gui_;
      deque<ca> scroll_data_; // for scroll up
      deque<ca> screen_data_;
      int cur_x_, cur_y_;
      int row_, col_;
      int screen_begin_; // point to screen_data line
      DS::TEmulationPasrer *te_; // terminal parser, if te==0, don't parse
      MyPty *mypty_;
  };
} // end namespace DS


#endif
