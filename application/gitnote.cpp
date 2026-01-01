#include "editorHandler/editorHandler.h"
#include "loginHandler/loginHandler.h"
#include "authHandler/authHandler.h"
#include "dashboardHandler/dashboardHandler.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("GitNote");
    QCoreApplication::setApplicationName("GitNoteClient");

    QWidget* window = nullptr;

    if(authHandler::loadToken().isEmpty()){
        qDebug() << "No valid token was found";
        window = new loginHandler();        // heap allocation
    }else{
        qDebug() << "Token was found";
        window = new dashboardHandler();    // heap allocation
    }

    window->show();

    return a.exec();

}
