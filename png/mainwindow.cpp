#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sett = new (Settings);
    img = new Image();
    picture = new MyGraphicView();
    //ui->gridLayout->addWidget(picture);
    connect(picture, SIGNAL(selection()), this, SLOT(selection()));
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
    if (img->open_png_file(file_name.toLocal8Bit().constData())){
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть изображение");
    }
    //picture->new_size(img->Bitmap.width,img->Bitmap.height);
    if(img->Bitmap.color_type!=6){
        QMessageBox::critical(this, "YOU SHAL NOT PASS", "ЭТО НЕ RGBA");
        return;
    }
    filename=file_name;
    picture->update(img->get_pixmap());
    ui->gridLayout->addWidget(picture);
    //QMessageBox::critical(this, "Ошибка", file_name);
}

void MainWindow::on_save_triggered()
{
    QString save_name = QFileDialog::getSaveFileName(this, "Выберите файл для открытия", "/home/kot/PNG_CW/img", "*.png");
    if(img->save_png_file(save_name.toLocal8Bit().constData())){
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить изображение");
        return;
    }
}

void MainWindow::on_Inversion_clicked()
{
    if (img->Bitmap.width == 0 || img->Bitmap.height == 0){
        QMessageBox::critical(this, "Ошибка", "Для использования инструментов загрузите изображение");
        return;
    }
    button_pressed = INVERSE;
    picture->button_pressed=INVERSE;

}

void MainWindow::on_Rectangle_clicked()
{
    if (img->Bitmap.width == 0 || img->Bitmap.height == 0){
        QMessageBox::critical(this, "Ошибка", "Для использования инструментов загрузите изображение");
        return;
    }
    button_pressed = RECTANGLE;
    picture->button_pressed = RECTANGLE;
    //color = QColorDialog::getColor();
    //picture->color = color;
    //if (!color.isValid()) return;

}

void MainWindow::on_refrsh_triggered()
{
    if (img->open_png_file(filename.toLocal8Bit().constData())){
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть изображение");
    }
    picture->update(img->get_pixmap());
    ui->gridLayout->addWidget(picture);
}

void MainWindow::on_exit_triggered()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,"Прощай","Ты хочешь покинуть меня?", QMessageBox::Yes | QMessageBox::No);
    if(reply==QMessageBox::Yes){
        QApplication::quit();
    }
}

void MainWindow::selection(){
    switch (button_pressed) {
        case INVERSE:
            if (picture->click_coordinate.x1 < 0 || picture->click_coordinate.x1 >= static_cast<int>(img->Bitmap.width) ||
                picture->click_coordinate.x2 < 0 || picture->click_coordinate.x2 >= static_cast<int>(img->Bitmap.width) ||
                picture->click_coordinate.y1 < 0 || picture->click_coordinate.y1 >= static_cast<int>(img->Bitmap.height) ||
                picture->click_coordinate.y2 < 0 || picture->click_coordinate.y2 >= static_cast<int>(img->Bitmap.height)){
                QMessageBox::critical(this, "Ошибка", "Неккоректно задана область");
                return;
            }
            img->negativ_RGB(picture->click_coordinate.x1, picture->click_coordinate.y1, picture->click_coordinate.x2, picture->click_coordinate.y2);
            picture->update(img->get_pixmap());
            ui->gridLayout->addWidget(picture);
            break;
    case RECTANGLE:
        if (picture->click_coordinate.x1 < 0 || picture->click_coordinate.x1 >= static_cast<int>(img->Bitmap.width) ||
            picture->click_coordinate.x2 < 0 || picture->click_coordinate.x2 >= static_cast<int>(img->Bitmap.width) ||
            picture->click_coordinate.y1 < 0 || picture->click_coordinate.y1 >= static_cast<int>(img->Bitmap.height) ||
            picture->click_coordinate.y2 < 0 || picture->click_coordinate.y2 >= static_cast<int>(img->Bitmap.height)){
            QMessageBox::critical(this, "Ошибка", "Неккоректно задана область");
            return;
        }
        if(sett->rec_line_size==0){
            QMessageBox::warning(this, "Неа", "Недопустимая толщина");
            return ;
        }
        key=img->print_rectangle(picture->click_coordinate.x1, picture->click_coordinate.y1, picture->click_coordinate.x2, picture->click_coordinate.y2,sett->rec_line_size,sett->rec_line_color);
        if(key==-1){
            QMessageBox::critical(this, "Ошибка", "Недопустимые размеры области при заданной толщина");
            return;
        }

        if(sett->fill){
            img->rec_fill(picture->click_coordinate.x1, picture->click_coordinate.y1, picture->click_coordinate.x2, picture->click_coordinate.y2,sett->rec_line_size,sett->rec_fill_color);
        }
        picture->update(img->get_pixmap());
        ui->gridLayout->addWidget(picture);
        break;
    case SPLIT:
        if(sett->vertical_count==0||sett->gorizontal_count==0||sett->line_split_size==0){
            QMessageBox::critical(this, "Ошибка", "Недопустимые настройки разделения фото");
        }
        img->division(sett->vertical_count,sett->gorizontal_count,sett->line_split_size,sett->split_color);
        picture->update(img->get_pixmap());
        ui->gridLayout->addWidget(picture);
        break;
    case ROTATE:
        if (picture->click_coordinate.x1 < 0 || picture->click_coordinate.x1 >= static_cast<int>(img->Bitmap.width) ||
            picture->click_coordinate.x2 < 0 || picture->click_coordinate.x2 >= static_cast<int>(img->Bitmap.width) ||
            picture->click_coordinate.y1 < 0 || picture->click_coordinate.y1 >= static_cast<int>(img->Bitmap.height) ||
            picture->click_coordinate.y2 < 0 || picture->click_coordinate.y2 >= static_cast<int>(img->Bitmap.height)){
            QMessageBox::critical(this, "Ошибка", "Неккоректно задана область");
            return;
        }
        img->rotate(picture->click_coordinate.x1, picture->click_coordinate.y1, picture->click_coordinate.x2, picture->click_coordinate.y2,90);
        picture->update(img->get_pixmap());
        ui->gridLayout->addWidget(picture);
    }
}



void MainWindow::on_Options_clicked()
{
    sett->exec();
}


void MainWindow::on_Split_clicked()
{
   // QApplication::quit();
    //QMessageBox::critical(this, "Ошибка", "хули");
    if (img->Bitmap.width == 0 || img->Bitmap.height == 0){
        QMessageBox::critical(this, "Ошибка", "Для использования инструментов загрузите изображение");
        return;
    }
    button_pressed = SPLIT;
    //picture->button_pressed = SPLIT;
}

void MainWindow::on_Rotate_clicked()
{
    if (img->Bitmap.width == 0 || img->Bitmap.height == 0){
        QMessageBox::critical(this, "Ошибка", "Для использования инструментов загрузите изображение");
        return;
    }
    button_pressed = ROTATE;
    picture->button_pressed = ROTATE;
}


