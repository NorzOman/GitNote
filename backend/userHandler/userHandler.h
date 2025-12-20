
#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <utility>
#include "../databaseHandler/sqlite_modern_cpp.h"

struct userInfo{
    bool validUser = false;
    std::string username = "guest_user";
    std::vector<std::pair<std::string,std::string>> notes;
};

std::string registerUser(
    std::string& username,
    std::string& password,
    sqlite::database& userDb
);

std::string loginUser(
    std::string& username,
    std::string& password,
    sqlite::database& userDb
);

bool verifyToken(
    std::string& token,
    sqlite::database& userDb
);

userInfo userLookup(
    std::string& token,
    sqlite::database& userDb,
    sqlite::database& dataDb
);
