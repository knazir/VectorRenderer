#include "MainWindow.h"

#include "CanvasWidget.h"

// Renderer
#include <Renderer/IRenderDevice.h>

// Vector
#include <Vector/VectorShape.h>

// External
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

    mCanvas = new CanvasWidget(GraphicsBackend::DirectX, this);
    setCentralWidget(mCanvas);

    CreateTestShapes();
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete mCanvas;
    mCanvas = nullptr;
}

//------------------------------------------------------------------------------
void MainWindow::CreateTestShapes()
{
    // Create a red line
    Line* line = new Line(50.0f, 100.0f, 300.0f, 100.0f);
    line->SetStroke(1.0f, 0.0f, 0.0f, 1.0f, 5.0f);

    // Create a green rectangle
    Rect* rect = new Rect(100.0f, 150.0f, 200.0f, 100.0f);
    rect->SetFill(0.0f, 1.0f, 0.0f, 1.0f);

    // Create a blue quadratic Bezier curve
    BezierCurve* curve = new BezierCurve(50.0f, 300.0f, 300.0f, 300.0f, 150.0f, 400.0f, 0.0f, 0.0f);
    curve->SetStroke(0.0f, 0.0f, 1.0f, 1.0f, 2.0f);

    // Add shapes to the renderer
    mCanvas->AddShape(line);
    mCanvas->AddShape(rect);
    mCanvas->AddShape(curve);
}
