#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

//------------------------------------------------------------------------------
class CanvasWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	CanvasWidget(QWidget* parent = nullptr);
	~CanvasWidget();

protected:
	void initializeGL() override;
	void resizeGL(int32_t width, int32_t height) override;
	void paintGL() override;
};

