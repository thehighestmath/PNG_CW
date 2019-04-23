#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->right_rot->setChecked(1);
    ui->rec_line_size->setValue(1);
    ui->vertical_count->setValue(1);
    ui->gorizontal_count->setValue(1);
    ui->line_split_size->setValue(1);

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

void Settings::on_right_rot_clicked()
{
    if(ui->right_rot->isChecked()){
        angle=90;
    }
}

void Settings::on_half_rot_clicked()
{
    if(ui->half_rot->isChecked()){
        angle=180;
    }
}



void Settings::on_left_rot_clicked()
{
    if(ui->left_rot->isChecked())
        angle=270;
}

void Settings::on_display_clicked()
{
    if(ui->display->isChecked())
        angle=360;
}

void Settings::on_exit_clicked()
{
    this->close();
}

void Settings::on_color_fill_pattern_clicked()
{
    color_fill_pattern=QColorDialog::getColor();
    //picture->color = color;
    if (!rec_line_color.isValid()) return;
}

void Settings::on_color_pattern_clicked()
{
    color_pattern=QColorDialog::getColor();
    //picture->color = color;
    if (!rec_line_color.isValid()) return;
}
