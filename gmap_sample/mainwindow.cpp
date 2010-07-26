#include "mainwindow.h"
#include <QVBoxLayout>
#include <QDir>
#include <QWebView>
#include <QHeaderView>

#include "addressmodel.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags)
{
    ui.setupUi(this);

    ui.treeView->setModel(new AddressModel(this));
    ui.treeView->header()->setStretchLastSection(true);

    connect(ui.treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(showItem(QModelIndex)));
    ui.map->show();
}

MainWindow::~MainWindow()
{

}

void MainWindow::showItem(const QModelIndex &idx)
{
    ui.map->clearCoordinates();
    ui.map->geoCode( idx.data( Qt::UserRole).toString() );
    ui.nameLabel->setText(idx.data( Qt::DisplayRole ).toString());
    ui.addressLabel->setText(idx.data( Qt::UserRole).toString());
}
