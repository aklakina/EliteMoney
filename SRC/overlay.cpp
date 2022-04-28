#include "overlay.h"

Overlay::Overlay(QWidget *parent) : QWidget(parent)
{
    setWindowFlags( windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowStaysOnTopHint );
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    tmr.setInterval(250);
    connect(&tmr, &QTimer::timeout, [this](){
       update();
    });
    tmr.start();
    showFullScreen();
    layout = new QFormLayout(this);
    KillsSoFar=new QLabel();
    StackHeight=new QLabel();
    progress=new QLabel();
    stackHeightData=new QLabel();
    KillsSoFar->setText("Kills so far: ");
    StackHeight->setText("Stack height");
    progress->setText("0");
    stackHeightData->setText("0");
    layout->addRow(KillsSoFar,progress);
    layout->addRow(StackHeight,stackHeightData);
    layout->setGeometry(QRect(0,0,200,200));
}

void Overlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    //painter.fillRect(px,py,wx,wy,Qt::red);

    KillsSoFar->move(px,py);
    StackHeight->move(px,py+(dy));
    progress->move(px+(dx),py);
    stackHeightData->move(px+(dx),py+(dy));

    //++px;
    //++py;
}

