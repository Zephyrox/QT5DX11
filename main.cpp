#include "mainwindow.h"
#include <QApplication>
#include "dxframe.h"
#include "dx.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    DX dx;
    QFrame * frame = nullptr;

    w.setFixedWidth(1920);
    w.setFixedHeight(1080);

    QWidget * btn1 = w.findChild<QWidget*>("pushButton");
    QWidget * btn2 = w.findChild<QWidget*>("pushButton_2");

    bool method1 = true; //CHANGE THIS

    //METHOD 1 - Works but slow
    // - Render DX to texture, draw texture in frame paintEvent
    if(method1){
        dx.initDX((HWND)w.winId(), w.width(), w.height());

        frame = new DXFrame();
        w.setCentralWidget(frame);

        //add to frame or else it will be on top of the buttons
        btn1->setParent(frame);
        btn2->setParent(frame);
    }

    //METHOD 2 - Almost works, fast but can produce flicker
    // - Render DX on frame, place controls on top of frame (not in)
    // - Move the mouse between the buttons like crazy. A flicker occurs as a box between them.
    // - When VSync is ON, this method is actually slower on my machine. This is of course because nothings being rendered.
    if(!method1){
        frame = (QFrame *)w.findChild<QWidget*>("frame");

        dx.initDX((HWND)frame->winId(), w.width(), w.height());

        btn1->setParent(w.window());
        btn2->setParent(w.window());
    }

    w.show();

    while(w.running){
        a.processEvents();

        dx.render(method1);

        if(method1){
            ((DXFrame *)frame)->bitmap = dx.bitmap;
            frame->update();
        }
        w.setWindowTitle(QString::number(dx.FPS));
    }

    return a.exit();
}
