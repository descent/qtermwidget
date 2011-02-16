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
    void set_attr(const MapAttribute & attr)
    {
      attr_=attr; // all fields are objects, need spend much time.
    }
    const MapAttribute &get_attr() const
    {
      return attr_;
    }

  private:
    MapAttribute attr_;
};
#endif


