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
    layout = new QGridLayout(this);
    CombatStatProperties=new UiProperties(&CombatStats);
    CombatStatProperties->insert(0,0,new QLabel("0"));
    CombatStatProperties->insert(0,1,CreateIconLabel(":/icons/Resources/Logos/Mission.svg","#FF7100",20,20));
    CombatStatProperties->insert(1,0,new QLabel("0"));
    CombatStatProperties->insert(1,1,CreateIconLabel(":/icons/Resources/Logos/Money.svg","#0A8BD6",20,20));
    CombatStatProperties->addToLayout(layout);
    layout->setGeometry(QRect(0,0,0,0));
}

Overlay::~Overlay() {
    for (auto label:CombatStats) {
        delete label.second;
    }
}

void Overlay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    CombatStatProperties->move();
}

void Overlay::showAll()
{
    //Not implemented yet
}

std::string Overlay::GetUserConfig()
{
    json conf;
    json & CombatStats=conf["Combat Statistic config"];
    CombatStats["Px"]=CombatStatProperties->getPx();
    CombatStats["Py"]=CombatStatProperties->getPy();
    CombatStats["Dx"]=CombatStatProperties->getDx();
    CombatStats["Dy"]=CombatStatProperties->getDy();
    CombatStats["ODy"]=CombatStatProperties->getODy();
    return conf.dump(4);
}

QLabel* Overlay::CreateIconLabel(QString resourcepath, QString _color,unsigned w,unsigned h) {
    auto labelIcon=new QLabel();
    QIcon icon(resourcepath);
    QPixmap pixmap = icon.pixmap(QSize(w,h));
    QImage tmp = pixmap.toImage();
    QColor color(_color);
    for(int y = 0; y < tmp.height(); y++) {
        for(int x= 0; x < tmp.width(); x++) {
            color.setAlpha(tmp.pixelColor(x,y).alpha());
            tmp.setPixelColor(x,y,color);
        }
    }
    pixmap = QPixmap::fromImage(tmp);
    labelIcon->setPixmap(pixmap);
    labelIcon->setContentsMargins(0,0,20,20);
    return labelIcon;
}
