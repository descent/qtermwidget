#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H

#include <QtGui>
#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QCursor>

class ConfWidget : public QWidget
{
  Q_OBJECT
  public:
    ConfWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
    //~ConfWidget();
  public slots:
    void move_cursor();
  protected:
    void mousePressEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );

    void keyReleaseEvent ( QKeyEvent * event );
  private:
    QPushButton *button_;
    QTimer *timer_;
    QCursor *m_cursor_;
    int x_diff_, y_diff_;
};

#endif
