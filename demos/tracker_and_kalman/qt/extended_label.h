#ifndef EXTENDED_LABEL_H
#define EXTENDED_LABEL_H

#include <QLabel>       // include this
#include <QMouseEvent>  // include this
#include <QEvent>
#include <QDebug>       // include this

class extended_label : public QLabel
{
    Q_OBJECT
public:
    explicit extended_label(QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);

    const int getX() {return X;}
    const int getY() {return Y;}

signals:

    void mouse_pressed();
    void mouse_pos();

public slots:

protected:

    int X;
    int Y;

};

#endif // EXTENDED_LABEL_H
