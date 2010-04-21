/*
 * only for inherit to show popup menu (use mouse right key).
 *
 */

#ifndef TREE_VIEW_POPUP_MENU_H
#define TREE_VIEW_POPUP_MENU_H


#include <QTreeWidgetItem>
#include <QTimer>
#include <QMenu>
#include <QFontDialog>


extern const int BIG5;
extern const int UTF8;
extern const char *encoding_name[];

// for convenient to create popup menu
#define CREATE_ACTIONS(qa, qa_title, slot_func) \
{ \
      qa = new QAction(tr(qa_title), this);\
      add_action(qa);\
      connect(qa, SIGNAL(triggered()), this, SLOT(slot_func() ) );\
}



class TreeViewPopupMenu: public QTreeWidget
{

  Q_OBJECT

  public:
    TreeViewPopupMenu( QWidget * parent=0);
    const char *get_cur_enc_str()
    {
      return encoding_name[cur_enc_];
    }
    int dump_raw_str(const QString &str);

  public slots:
    void s_more_info();

    void font_select()
    {
      setFont(QFontDialog::getFont(0, font()));
    }
    virtual void show_fo_menu()
    { // add a font select menu
      qDebug("TreeViewPopupMenu: show_fo_menu");
      m_->popup(QCursor::pos());
    }
    void s_big5_enc()
    {
      qDebug("AAA");
      cur_enc_ = BIG5;
      change_encode_fn();
    }
    void s_utf8_enc()
    {
      qDebug("BBB");
      cur_enc_ = UTF8;
      change_encode_fn();
    }



  protected:
    void change_encode_fn();
    void add_action(QAction *action)
    {
      m_->addAction(action);
    }
    void mousePressEvent ( QMouseEvent * e)
    {
      QTreeWidget::mousePressEvent( e );
      //QTimer::singleShot(1000, this, SLOT(show_fo_menu()));
      popup_menu_timer_->start(1000);
    }

    void mouseReleaseEvent( QMouseEvent * e)
    {
      QTreeWidget::mouseReleaseEvent( e );
      //qDebug("in mouseReleaseEvent");
      popup_menu_timer_->stop();
    }
    QMenu *m_, *encode_m_;

  protected:
    QTimer *popup_menu_timer_;
    QAction *font_select_;
    QAction *big5_enc_, *utf8_enc_;
    QActionGroup *enc_;
    QAction *more_info_;
    int cur_enc_;
  private:




};


#endif
