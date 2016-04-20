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
    int button = ev->button();
    if (button == 1) // left button
    {
        emit left_mouse_pressed();
    }
    else if (button == 2) // right button
    {
        emit right_mouse_pressed();
    }
    else
    {
        qDebug() << "A non-accepted button was pressed!!";
    }
}
