#pragma once

#include "ui_MainWindow.h"

#include <QtWidgets/QMainWindow>

//------------------------------------------------------------------------------
class CanvasWidget;

//------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::VectorRendererClass mUI;
    CanvasWidget* mCanvas = nullptr;
};
