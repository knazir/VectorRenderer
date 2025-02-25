// Application
#include "Application/MainWindow.h"

// External
#include <External/RenderDoc/renderdoc_app.h>
#include <QtWidgets/QApplication>

// System
#include <windows.h>

// Utils
#include <Utils/Assert.h>
#include <Utils/Config.h>

// Pointer to RenderDoc API
//------------------------------------------------------------------------------
RENDERDOC_API_1_6_0* gRenderDocAPI = nullptr;

//------------------------------------------------------------------------------
void InitializeRenderDoc()
{
    HMODULE renderDocModuile = LoadLibrary(L"renderdoc.dll");
    if (renderDocModuile)
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(renderDocModuile, "RENDERDOC_GetAPI");
        if (RENDERDOC_GetAPI)
        {
            RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_6_0, (void**)&gRenderDocAPI);
        }
    }

    if (gRenderDocAPI)
    {
        gRenderDocAPI->MaskOverlayBits(eRENDERDOC_Option_DebugOutputMute, 1);
    }
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
#ifdef RENDER_DOC
    InitializeRenderDoc();
#endif

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
