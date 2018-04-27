#ifndef DXFRAME_H
#define DXFRAME_H

#include <QApplication>
#include <QStylePainter>
#include <QWidget>
#include <QFrame>

class DXFrame : public QFrame
{


public:
    DXFrame(){
        //this->setAutoFillBackground(false);
    }

    void paintEvent(QPaintEvent* event){
        QStylePainter 	painter(this);
        painter.drawImage(0, 0, bitmap);
    }

    QImage						bitmap;

private:

};

#endif // DXFRAME_H
