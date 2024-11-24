#pragma once

#include "RendererFactory.h"

#include <QWidget>

//------------------------------------------------------------------------------
class IRenderer;

//------------------------------------------------------------------------------
class CanvasWidget : public QWidget
{
	Q_OBJECT

public:
	CanvasWidget(GraphicsBackend backend, QWidget* parent = nullptr);
	~CanvasWidget();

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual QPaintEngine* paintEngine() const override { return nullptr; }

private:
	IRenderer* mRenderer = nullptr;
};

