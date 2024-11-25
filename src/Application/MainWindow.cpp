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
    // Create a line
    Line* line = new Line(960.0f, 540.0f, 1920.0f, 1080.0f);
    line->SetStroke(1.0f, 0.0f, 0.0f, 1.0f, 5.0f);

    // Create a rectangle
    Rect* rect = new Rect(0.0f, 0.0f, 960.0f, 540.0f);
    rect->SetFill(1.0f, 1.0f, 1.0f, 1.0f);

    // Create a quadratic Bezier curve
    BezierCurve* quadCurve = new BezierCurve(960.0f, 540.0f, 0.0f, 1080.0f, 480.0f, 1010.0f);
    quadCurve->SetStroke(0.0f, 1.0f, 0.0f, 1.0f, 5.0f);

    // Create a cubic Bezier curve
    CubicBezierCurve* cubicCurve = new CubicBezierCurve(960.0f, 540.0f, 1920.0f, 0.0f, 1200.0f, 205.0f, 1440.0f, 335.0f);
    cubicCurve->SetStroke(0.0f, 0.0f, 1.0f, 1.0f, 5.0f);

    // Add shapes to the renderer
    mCanvas->AddShape(line);
    mCanvas->AddShape(rect);
    mCanvas->AddShape(quadCurve);
    mCanvas->AddShape(cubicCurve);
}
