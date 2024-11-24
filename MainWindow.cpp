#include "MainWindow.h"

#include "CanvasWidget.h"

#include <QIcon>

//------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    mUI.setupUi(this);

    setWindowTitle("Vector Renderer");
    resize(800, 600);

    mUI.menuBar->hide();
    mUI.mainToolBar->hide();
    mUI.statusBar->showMessage("Ready");

    mCanvas = new CanvasWidget(this);
    setCentralWidget(mCanvas);
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete mCanvas;
    mCanvas = nullptr;
}
