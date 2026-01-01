
#pragma once

#include <QString>

class authHandler {
public:
    static void saveToken(const QString& token);
    static QString loadToken();
    static void clearToken();
};
