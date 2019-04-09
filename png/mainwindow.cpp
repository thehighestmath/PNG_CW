#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    img = new Image();
    picture = new MyGraphicView();
    //ui->gridLayout->addWidget(picture);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Выберите файл для открытия", "/home/kot/PNG_CW/img", "*.png");
    if (file_name == nullptr){
        return;
    }
    if (img->read_png_file(file_name.toLocal8Bit().constData())){
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть изображение");
    }
    picture->update(img->get_pixmap());
    ui->gridLayout->addWidget(picture);
    //QMessageBox::critical(this, "Ошибка", file_name);
}
