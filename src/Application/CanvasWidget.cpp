#include "CanvasWidget.h"

// Renderer
#include "Renderer/RendererFactory.h"

//------------------------------------------------------------------------------
CanvasWidget::CanvasWidget(GraphicsBackend backend, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);

	mRenderer = RendererFactory::Create(backend);
	if (mRenderer == nullptr)
	{
		qWarning() << "Failed to create renderer";
		return;
	}

	mRenderer->Initialize(reinterpret_cast<void*>(winId()), width(), height());
}

//------------------------------------------------------------------------------
CanvasWidget::~CanvasWidget()
{
	if (mRenderer != nullptr)
	{
		mRenderer->Shutdown();
		delete mRenderer;
		mRenderer = nullptr;
	}
}

//------------------------------------------------------------------------------
void CanvasWidget::paintEvent(QPaintEvent* /*event*/)
{
	if (mRenderer != nullptr)
	{
		mRenderer->Clear(0.5f, 0.5f, 0.5f, 1.0f);
		mRenderer->Render();
	}
}

//------------------------------------------------------------------------------
void CanvasWidget::resizeEvent(QResizeEvent* /*event*/)
{
	if (mRenderer != nullptr)
	{
		mRenderer->Resize(width(), height());
	}
}
