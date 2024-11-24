#include "CanvasWidget.h"

// Renderer
#include <Renderer/RendererFactory.h>
#include <Renderer/VectorRenderer.h>

// Utils
#include <Utils/Assert.h>

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

	mRenderDevice = RendererFactory::Create(backend);
	const bool initialized = mRenderDevice->Initialize(reinterpret_cast<void*>(winId()), width(), height());
	ASSERT(initialized, "Failed to initialize render device");

	const bool shadersLoaded = mRenderDevice->LoadShaders();
	ASSERT(shadersLoaded, "Failed to load shaders");

	mVectorRenderer = new VectorRenderer(mRenderDevice);

	// Start update loop
	connect(mTimer, &QTimer::timeout, this, &CanvasWidget::Update);
	mTimer->start(1000.0f / 60.0f); // ~60 FPS (TODO: Make configurable)
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
