#ifndef OVERLAYEDITOR_H
#define OVERLAYEDITOR_H

#include <QMainWindow>
#include "overlay.h"

namespace Ui {
class OverlayEditor;
}

class OverlayEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit OverlayEditor(QWidget *parent = nullptr);
    OverlayEditor(Overlay* ovl,QWidget *parent=nullptr);
    ~OverlayEditor();

private slots:
    void on_verticalSlider_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_verticalSlider_2_valueChanged(int value);

    void on_verticalSlider_3_valueChanged(int value);

private:
    Ui::OverlayEditor *ui;
    Overlay * ol;
};

#endif // OVERLAYEDITOR_H
