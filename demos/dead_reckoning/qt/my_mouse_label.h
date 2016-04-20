#ifndef MY_MOUSE_LABEL_H
#define MY_MOUSE_LABEL_H

#include <QWidget>
#include <QLabel>      // include this
#include <QMouseEvent> // include this
#include <QEvent>
#include <QDebug>      // include this

class my_mouse_label : public QLabel
{
    Q_OBJECT
public:
    explicit my_mouse_label(QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);

    const int getX() {return X;}
    const int getY() {return Y;}

    // ======================================================================
    // Plot stuff
    // ======================================================================

    // Plot the data
    void plot(const unsigned x_max, const unsigned y_max);

signals:

    void left_mouse_pressed();
    void right_mouse_pressed();
    void mouse_pos();

public slots:

protected:

    int X;
    int Y;
};

#endif // MY_MOUSE_LABEL_H
