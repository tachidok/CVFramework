#include "my_mouse_label.h"

my_mouse_label::my_mouse_label(QWidget *parent) :
    QLabel(parent)
{ }

void my_mouse_label::mouseMoveEvent(QMouseEvent *ev)
{
    this->X = ev->x();
    this->Y = ev->y();

    emit mouse_pos();
}

void my_mouse_label::mousePressEvent(QMouseEvent *ev)
{
    emit mouse_pressed();
}
