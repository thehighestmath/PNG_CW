#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QPixmap>
#include "mygraphicview.h"
#include "image.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_open_triggered();

private:
    Ui::MainWindow *ui;
    Image *img;
    MyGraphicView *picture;
    QGraphicsScene *scene;
};

#endif // MAINWINDOW_H
