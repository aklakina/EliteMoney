#ifndef OVERLAY_H
#define OVERLAY_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QLabel>

class Overlay : public QWidget
{
    Q_OBJECT
public:
    explicit Overlay(QWidget *parent = nullptr);
    //void paintEvent(QPaintEvent* event) override;
    QTimer tmr;
    QLabel *KillsSoFar
    ,*StackHeight
    ,*progress
    ,*stackHeightData;
signals:

};

#endif // OVERLAY_H
