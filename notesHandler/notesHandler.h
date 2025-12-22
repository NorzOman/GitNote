
#pragma once

#include <string>
#include "../databaseHandler/sqlite_modern_cpp.h"

std::string createNote(
    std::string& token,
    std::string& title,
    sqlite::database &dataDb
);

bool saveNote(
    std::string& token,
    std::string& duuid,
    std::string& content,
    sqlite::database &dataDb
);

std::string viewNote(
    std::string& token,
    std::string& duuid,
    sqlite::database &dataDb
);
