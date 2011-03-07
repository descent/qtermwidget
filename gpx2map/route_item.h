#ifndef ROUTE_ITEM_H
#define ROUTE_ITEM_H

#include <QTreeWidgetItem>

struct MapAttribute
{
  QString name, desc, clickable, width, opacity;
  QString outline_color, outline_width, fill_color, fill_opacity;
  int color; // a index, not color number
  QColor qc;
  QString points;
};


class RouteItem: public QTreeWidgetItem
{
  public:
    RouteItem(QTreeWidget * parent, int type = Type):QTreeWidgetItem(parent, Type)
    {
    }

    void set_rn(const QString &rn)
    {
      attr_.name=rn;
    }

    void set_qc(const QColor &qc)
    {
      attr_.qc=qc;
    }
    const QColor &get_qc() const
    {
      return attr_.qc;
    }
    void set_attr(const MapAttribute & attr)
    {
      attr_=attr; // all fields are objects, need spend much time.
    }
    const MapAttribute &get_attr() const
    {
      return attr_;
    }
    void update_text()
    {
      setText(1, attr_.name);
      QString hc;
      bool qcolor2html_color_str(const QColor &qc, QString &html_color_str);

      qcolor2html_color_str(attr_.qc, hc); 
      setText(2, hc);

      setForeground(2, attr_.qc);

    }


  private:
    MapAttribute attr_;
};
#endif


