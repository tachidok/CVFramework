#include "extended_label.h"

extended_label::extended_label(QWidget *parent) :
    QLabel(parent)
{
}

void extended_label::mouseMoveEvent(QMouseEvent *ev)
{
    this->X = ev->x();
    this->Y = ev->y();

    //qDebug() << X << Y;

    emit mouse_pos();
}

void extended_label::mousePressEvent(QMouseEvent *ev)
{
    emit mouse_pressed();
}
