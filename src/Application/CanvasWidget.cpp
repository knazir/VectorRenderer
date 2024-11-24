#include "CanvasWidget.h"

// Renderer
#include <Renderer/RendererFactory.h>
#include <Renderer/VectorRenderer.h>

// External
#include <QTimer>

//------------------------------------------------------------------------------
CanvasWidget::CanvasWidget(GraphicsBackend backend, QWidget* parent)
	: QWidget(parent)
	, mTimer(new QTimer(nullptr))
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	show();

	// Initialize render device
	mRenderDevice = RendererFactory::Create(backend);
	if (mRenderDevice == nullptr)
	{
		exit(EXIT_FAILURE);
	}
	const bool initialized = mRenderDevice->Initialize(reinterpret_cast<void*>(winId()), width(), height());
	if (!initialized)
	{
		exit(EXIT_FAILURE);
	}

	// Initialize vector renderer
	mVectorRenderer = new VectorRenderer(mRenderDevice);

	// Start update loop
	connect(mTimer, &QTimer::timeout, this, &CanvasWidget::Update);
	mTimer->start(16); // 60 FPS (TODO: Make configurable)
}

//------------------------------------------------------------------------------
CanvasWidget::~CanvasWidget()
{
	disconnect(mTimer, &QTimer::timeout, this, &CanvasWidget::Update);
	delete mTimer;
	mTimer = nullptr;

	delete mVectorRenderer;
	mVectorRenderer = nullptr;

	mRenderDevice->Shutdown();
	delete mRenderDevice;
	mRenderDevice = nullptr;
}

//------------------------------------------------------------------------------
void CanvasWidget::AddShape(IVectorShape* shape)
{
	mVectorRenderer->AddShape(shape);
}

//------------------------------------------------------------------------------
void CanvasWidget::ClearShapes()
{
	mVectorRenderer->ClearShapes();
}

//------------------------------------------------------------------------------
void CanvasWidget::resizeEvent(QResizeEvent* /*event*/)
{
	mRenderDevice->Resize(width(), height());
}

//------------------------------------------------------------------------------
void CanvasWidget::Update()
{
	mVectorRenderer->Render();
	update();
}
