#ifndef MYGRAPHICVIEW_H
#define MYGRAPHICVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QColor>

#define INVERSE 1
#define ROTATE 2
#define RECTANGLE 3
#define SPLIT 4

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

    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void update(QPixmap pixmap);

signals:
    void selection();

private:
    QGraphicsScene      *scene;     // Объявляем сцену для отрисовки
    QGraphicsItemGroup  *group;   // Объявляем первую группу элементов
    QPixmap             pixmap;
    //QGraphicsItemGroup  *group_2;   // Объявляем вторую группу элементов
    //void resizeEvent(QResizeEvent *event);
    void deleteItemsFromGroup(QGraphicsItemGroup *group1);


};

#endif // MYGRAPHICVIEW_H
