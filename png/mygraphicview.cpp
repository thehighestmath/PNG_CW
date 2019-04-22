#include "mygraphicview.h"
#include "mainwindow.h"

MyGraphicView::MyGraphicView(QWidget *parent) : QGraphicsView(parent)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    this->setMinimumHeight(100);
    this->setMinimumWidth(100);

    group = new QGraphicsItemGroup();
}

void MyGraphicView::update(QPixmap pixmap){
    scene = new QGraphicsScene();
    //scene->setSceneRect(0,0,width,height);
    scene->addPixmap(pixmap);
    this->setScene(scene);
}

void MyGraphicView::mousePressEvent(QMouseEvent* event)
{
    click_coordinate.x1 = this->mapFromGlobal(QCursor::pos()).x();
    click_coordinate.y1 = this->mapFromGlobal(QCursor::pos()).y();
}

void MyGraphicView::mouseReleaseEvent(QMouseEvent* event)
{
    click_coordinate.x2 = this->mapFromGlobal(QCursor::pos()).x();
    click_coordinate.y2 = this->mapFromGlobal(QCursor::pos()).y();
    this->deleteItemsFromGroup(group);
    emit selection();
}


void MyGraphicView::mouseMoveEvent(QMouseEvent *event)
{
    if (button_pressed == INVERSE || button_pressed == RECTANGLE || button_pressed == ROTATE)
    {
        int x1 = click_coordinate.x1;
        int y1 = click_coordinate.y1;
        int x2 = this->mapFromGlobal(QCursor::pos()).x();
        int y2 = this->mapFromGlobal(QCursor::pos()).y();
        this->deleteItemsFromGroup(group);
        group = new QGraphicsItemGroup();

        color.setRed(255);
        color.setGreen(255);
        color.setBlue(255);

        if (x2 < 0)
            x2 = 0;
        if (y2 < 0)
            y2 = 0;
        if (x2 < x1)
            std::swap(x1, x2);

        if (y2 < y1)
            std::swap(y1, y2);

        group->addToGroup((scene->addRect(x1, y1, x2 - x1, y2 - y1, color)));
        scene->addItem(group);
    }
}


void MyGraphicView::deleteItemsFromGroup(QGraphicsItemGroup *group)
{
    /* Перебираем все элементы сцены, и если они принадлежат группе,
     * переданной в метод, то удаляем их
     * */
    foreach( QGraphicsItem *item, scene->items(group->boundingRect())) {
       if(item->group() == group ) {
          delete item;
       }
    }
}

MyGraphicView::~MyGraphicView()
{

}
