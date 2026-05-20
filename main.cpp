#ifdef _WIN32
#include <windows.h>
#endif

#include <QApplication>
#include "src/qt/MainWindow.h"

int main(int argc, char *argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}