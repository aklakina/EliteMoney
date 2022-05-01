#ifndef OVERLAY_H
#define OVERLAY_H

#include "containerobject.h"

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QLabel>
#include <QGridLayout>
#include <QIcon>

using namespace techlevi;

using namespace std;

class Overlay : public QWidget
{
    Q_OBJECT


    void paintEvent(QPaintEvent* event) override;
    QTimer tmr;
    map<pair<unsigned,unsigned>,QLabel*> CombatStats;
    QGridLayout *layout;
    QLabel* CreateIconLabel(QString resourcepath,QString _color,unsigned w, unsigned h);
    //bool Visible=true;

public:
    explicit Overlay(QWidget *parent = nullptr);
    ~Overlay();

    UiProperties * CombatStatProperties;

    void showAll();

    json GetUserConfig();

    /*void toggleVisibility();

    void setVisibility(bool v);*/
signals:

};

#endif // OVERLAY_H
