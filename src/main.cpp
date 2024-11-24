// Application
#include "Application/MainWindow.h"

// External
#include <QtWidgets/QApplication>

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
