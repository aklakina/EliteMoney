#include "overlay.h"
#include <QFormLayout>

Overlay::Overlay(QWidget *parent) : QWidget(parent)
{
    setWindowFlags( windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowStaysOnTopHint );
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
/*
    tmr.setInterval(250);
    connect(&tmr, &QTimer::timeout, [this](){
       update();
    });
    tmr.start();
*/
    showFullScreen();

    KillsSoFar=new QLabel(this);
    StackHeight=new QLabel(this);
    progress=new QLabel(this);
    stackHeightData=new QLabel(this);
    KillsSoFar->setText("Kills so far: ");
    StackHeight->setText("Stack height");
    progress->setText("0");
    stackHeightData->setText("0");
    QFormLayout *layout = new QFormLayout();
    layout->addRow(KillsSoFar,progress);
    layout->addRow(StackHeight,stackHeightData);

    setLayout(layout);
}
/*

static int px = 0;
static int py = 0;
static int wx = 50;
static int wy = 50;

void Overlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(px,py,wx,wy,Qt::red);
    ++px;
    ++py;
}

*/
