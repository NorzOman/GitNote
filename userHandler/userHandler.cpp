
#include "userHandler.h"
#include "../databaseHandler/sqlite_modern_cpp.h"
#include <cstdlib>
#include <ctime>

std::string generateToken(int length){
    const std::string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string token;
    for(int i = 0; i < length ; ++i){
        int index = std::rand() % chars.size();
        token += chars[index];
    }
    return token;
}

std::string registerUser(
    std::string& username,
    std::string& password,
    sqlite::database& userDb
){
    int count = 0;
    userDb << "select count(*) from usersCreds where username = ?;"
    << username
    >> count;

    if(count > 0) return "-1";

    std::string token = generateToken(128);
    userDb << "INSERT INTO usersCreds (username, password , token ) VALUES (?, ? , ?);"
        << username << password << token;
    return token;
}

std::string loginUser(
    std::string &username,
    std::string &password,
    sqlite::database &userDb
){
    int count = 0;
    userDb << "select count(*) from usersCreds where username = ? and password = ?;"
    << username << password
    >> count;

    if(count == 0) return "-1";

    std::string token;
    userDb << "select token from usersCreds where username = ? and password = ?;"
    << username << password >> token;
    return token;
}

userInfo userLookup(
    std::string& token,
    sqlite::database& userDb,
    sqlite::database& dataDb
){
    userInfo u;
    std::string username;
    userDb << "select username from usersCreds where token = ?;" << token >> username;

    if(username.empty()) return u;

    u.validUser = true;
    u.username = username;

    dataDb << "SELECT dname, duuid FROM notesData WHERE token = ?;"
    << token
    >> [&](std::string dname, std::string duuid) {
        u.notes.emplace_back(dname, duuid);
    };

    return u;
}

bool verifyToken(std::string &token, sqlite::database &userDb){
    int count = 0;
    userDb << "select count(*) from usersCreds where token = ?;" << token >> count;
    if(count == 0) return false;
    else return true;
}
