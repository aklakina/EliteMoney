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
}

OverlayEditor::~OverlayEditor()
{
    delete ui;
}

void OverlayEditor::on_horizontalSlider_valueChanged(int value)
{
    qDebug()<<"px: "<<value;
    auto slider=ui->horizontalSlider;
    slider->setStatusTip(slider->statusTip().section(':',1,1)+QString::number(value));
    ol->px=value;
}


void OverlayEditor::on_verticalSlider_valueChanged(int value)
{
    qDebug()<<"py: "<<value;
    auto slider=ui->verticalSlider;
    slider->setStatusTip(slider->statusTip().section(':',1,1)+QString::number(value));
    ol->py=value;
}


void OverlayEditor::on_verticalSlider_2_valueChanged(int value)
{
    qDebug()<<"Dy: "<<value;
    auto slider=ui->verticalSlider_2;
    slider->setStatusTip(slider->statusTip().section(':',1,1)+QString::number(value));
    ol->dy=value;
}


void OverlayEditor::on_horizontalSlider_2_valueChanged(int value)
{
    qDebug()<<"Dx: "<<value;
    auto slider=ui->horizontalSlider_2;
    slider->setStatusTip(slider->statusTip().section(':',1,1)+QString::number(value));
    ol->dx=value;
}
