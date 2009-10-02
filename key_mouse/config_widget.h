#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H

#include <QtGui>
#include <QWidget>
#include <QPushButton>

class ConfWidget : public QWidget
{
  Q_OBJECT
  public:
    ConfWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
    //~ConfWidget();
  protected:
    void mousePressEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
  private:
    QPushButton *button_;
};

#endif
