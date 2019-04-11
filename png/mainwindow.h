#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QPixmap>
#include <QColorDialog>
#include "mygraphicview.h"
#include "image.h"
#include "settings.h"

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
    void on_Inversion_clicked();
    void on_exit_triggered();
    void on_save_triggered();
    void selection();
    void on_Rectangle_clicked();
    void on_pattern_2_clicked();
    void on_options_clicked();

    void on_Options_clicked();

private:
    Ui::MainWindow *ui;
    Image *img;
    MyGraphicView *picture;
    QGraphicsScene *scene;
    int button_pressed;
    QColor color;
    Settings *sett;
    int key;
};

#endif // MAINWINDOW_H
