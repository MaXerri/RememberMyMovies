
// ============== main.cpp (GUI Version) ==============
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Movie Review Manager");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Your Name");
    
    // No Docker auto-start. Expect the backend to be running separately.
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

