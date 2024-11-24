#pragma once

#include "ui_VectorRenderer.h"

#include <QtWidgets/QMainWindow>

//------------------------------------------------------------------------------
class VectorRenderer : public QMainWindow
{
    Q_OBJECT

public:
    VectorRenderer(QWidget* parent = nullptr);
    ~VectorRenderer();

private:
    Ui::VectorRendererClass mUI;
};
