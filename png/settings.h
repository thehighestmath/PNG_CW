#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QColorDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
    QColor rec_fill_color, rec_line_color, split_color;
    bool fill=false;
    int rec_line_size=1;
    int line_split_size=1;

    int gorizontal_count=1;
    int vertical_count =1;

private slots:
    void on_rec_line_color_clicked();

    void on_rec_fill_color_clicked();

    void on_split_color_clicked();

    void on_rec_line_size_valueChanged(int arg1);

    void on_rec_is_fill_stateChanged(int arg1);

    void on_gorizontal_count_valueChanged(int arg1);

    void on_vertical_count_valueChanged(int arg1);

    void on_line_split_size_valueChanged(int arg1);

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
