#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>
#include "ui_mainwindow.h"
#include <QWebView>
#include <QStandardItemModel>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~MainWindow();

public slots:
    void showItem(const QModelIndex& idx);

private:
    Ui::MainWindowClass ui;
};

#endif // MAINWINDOW_H
