#include "mygraphicview.h"
#include "mainwindow.h"

MyGraphicView::MyGraphicView(QWidget *parent) : QGraphicsView(parent)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    group = new QGraphicsItemGroup();
}

void MyGraphicView::update(QPixmap pixmap){
    scene = new QGraphicsScene();
    scene->addPixmap(pixmap);
    this->setScene(scene);
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
