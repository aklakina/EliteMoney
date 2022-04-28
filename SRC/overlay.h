#ifndef OVERLAY_H
#define OVERLAY_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QLabel>
#include <QFormLayout>
#include <QTableWidget>
#include <QListWidget>

class Overlay : public QWidget
{
    Q_OBJECT
public:
    explicit Overlay(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    QTimer tmr;
    QLabel *KillsSoFar
    ,*StackHeight
    ,*progress
    ,*stackHeightData
    ,*TargetFaction;
    QList<QLabel*> * CurrentShareableMissions;
    QFormLayout *layout;

    unsigned px=0,py=0,dy=50,dx=100;

signals:

};

#endif // OVERLAY_H
