#include "overlayeditor.h"
#include "ui_overlayeditor.h"

OverlayEditor::OverlayEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OverlayEditor)
{
    ui->setupUi(this);
}

OverlayEditor::OverlayEditor(Overlay *ovl, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OverlayEditor)
{
    ol=ovl;
    ui->setupUi(this);
    ol->showAll();
}

OverlayEditor::~OverlayEditor()
{
    delete ui;
}

void OverlayEditor::on_horizontalSlider_valueChanged(int value)
{
    qDebug()<<"px: "<<value;
    auto slider=ui->horizontalSlider;
    slider->setStatusTip(slider->statusTip().section(':',0,0)+": "+QString::number(value));
    ol->CombatStatProperties->setPx(value);
}


void OverlayEditor::on_verticalSlider_valueChanged(int value)
{
    qDebug()<<"py: "<<value;
    auto slider=ui->verticalSlider;
    slider->setStatusTip(slider->statusTip().section(':',0,0)+": "+QString::number(value));
    ol->CombatStatProperties->setPy(value);
}

void OverlayEditor::on_horizontalSlider_2_valueChanged(int value)
{
    qDebug()<<"Dx: "<<value;
    auto slider=ui->horizontalSlider_2;
    slider->setStatusTip(slider->statusTip().section(':',0,0)+": "+QString::number(value));
    ol->CombatStatProperties->setDx(value);
}

void OverlayEditor::on_verticalSlider_2_valueChanged(int value)
{
    qDebug()<<"Dy: "<<value;
    auto slider=ui->verticalSlider_2;
    slider->setStatusTip(slider->statusTip().section(':',0,0)+": "+QString::number(value));
    ol->CombatStatProperties->setDy(value);
}

void OverlayEditor::on_verticalSlider_3_valueChanged(int value)
{
    qDebug()<<"ODy: "<<value;
    auto slider=ui->verticalSlider_3;
    slider->setStatusTip(slider->statusTip().section(':',0,0)+": "+QString::number(value));
    ol->CombatStatProperties->setODy(value);
}

