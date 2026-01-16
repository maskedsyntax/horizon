#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "XrandrManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void applyConfig();
    void saveConfig();
    void reloadScreens();

private:
    void setupUi();
    void loadScreens();
    
    QGraphicsView *view;
    QGraphicsScene *scene;
    XrandrManager m_manager;
};

#endif // MAINWINDOW_H
