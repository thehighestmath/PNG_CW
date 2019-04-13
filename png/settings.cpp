#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_rec_line_color_clicked()
{
    rec_line_color = QColorDialog::getColor();
    //picture->color = color;
    if (!rec_line_color.isValid()) return;
}

void Settings::on_rec_fill_color_clicked()
{
    rec_fill_color = QColorDialog::getColor();
    //picture->color = color;
    if (!rec_fill_color.isValid()) return;
}

void Settings::on_split_color_clicked()
{
    split_color = QColorDialog::getColor();
    //picture->color = color;
    if (!split_color.isValid()) return;
}



void Settings::on_rec_line_size_valueChanged(int arg1)
{
    rec_line_size = ui->rec_line_size->value();
}

void Settings::on_rec_is_fill_stateChanged(int arg1)
{
    if(ui->rec_is_fill->isChecked()){
        fill = true;
    }
    else
        fill = false;
}

void Settings::on_gorizontal_count_valueChanged(int arg1)
{
    gorizontal_count = ui->gorizontal_count->value();
}

void Settings::on_vertical_count_valueChanged(int arg1)
{
    vertical_count = ui->vertical_count->value();
}

void Settings::on_line_split_size_valueChanged(int arg1)
{
    line_split_size = ui->line_split_size->value();
}
