
#include "authHandler.h"
#include <QSettings>

void authHandler::saveToken(const QString& token){
    QSettings settings;
    settings.setValue("auth/token" , token);
}

QString authHandler::loadToken(){
    QSettings settings;
    return settings.value("auth/token").toString();
}

void authHandler::clearToken(){
    QSettings settings;
    settings.remove("auth/token");
}
