#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QColor>

struct Coordinate{
    int x1=0;
    int y1=0;
    int x2=0;
    int y2=0;
};

class MyGraphicView : public QGraphicsView
{
    Q_OBJECT

public:

    explicit MyGraphicView(QWidget* parent = nullptr);
    ~MyGraphicView();

    int button_pressed = 0;
    QColor color;
    struct Coordinate click_coordinate;

    void update(QPixmap pixmap);


private:
    QGraphicsScene      *scene;     // Объявляем сцену для отрисовки
    QGraphicsItemGroup  *group;   // Объявляем первую группу элементов
    QPixmap             pixmap;
    //QGraphicsItemGroup  *group_2;   // Объявляем вторую группу элементов
    //void resizeEvent(QResizeEvent *event);
    void deleteItemsFromGroup(QGraphicsItemGroup *group1);


};

#endif // MYGRAPHICVIEW_H
