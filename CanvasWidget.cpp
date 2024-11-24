#include "CanvasWidget.h"

//------------------------------------------------------------------------------
CanvasWidget::CanvasWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{
}

//------------------------------------------------------------------------------
CanvasWidget::~CanvasWidget()
{
}

//------------------------------------------------------------------------------
void CanvasWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
}

//------------------------------------------------------------------------------
void CanvasWidget::resizeGL(int32_t width, int32_t height)
{
	glViewport(0, 0, width, height);
}

//------------------------------------------------------------------------------
void CanvasWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
