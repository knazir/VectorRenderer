#pragma once

// Renderer
#include <Renderer/RendererFactory.h>

// External
#include <QWidget>

//------------------------------------------------------------------------------
class IRenderDevice;
class IVectorShape;
class QTimer;
class VectorRenderer;

//------------------------------------------------------------------------------
class CanvasWidget : public QWidget
{
	Q_OBJECT

public:
	CanvasWidget(GraphicsBackend backend, QWidget* parent = nullptr);
	~CanvasWidget();

	void AddShape(IVectorShape* shape);
	void ClearShapes();

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual QPaintEngine* paintEngine() const override { return nullptr; }

private slots:
	void Update();

private:
	QTimer* mTimer = nullptr;
	IRenderDevice* mRenderDevice = nullptr;
	VectorRenderer* mVectorRenderer = nullptr;
};

