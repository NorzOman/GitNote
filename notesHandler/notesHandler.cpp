
#include "notesHandler.h"
#include <exception>
#include <string>
#include "../databaseHandler/sqlite_modern_cpp.h"

std::string generateUUID(int length){
    const std::string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string token;
    std::srand(std::time(nullptr));
    for(int i = 0; i < length ; ++i){
        int index = std::rand() % chars.size();
        token += chars[index];
    }
    return token;
}

std::string createNote(
    std::string& token,
    std::string& title,
    sqlite::database &dataDb
){
    std::string uuid = generateUUID(32);
    try{
        dataDb << "insert into notesData(token , dname , dcontent , duuid) values (?,?,?,?);"
        <<  token  << title << "Hello World!" << uuid;
        return uuid;
    } catch(std::exception& e){
        std::cout << "/notesHandler/notesHandler.cpp(createNote) : " << e.what() << std::endl;
        return "-1";
    }
}

bool saveNote(
    std::string& token,
    std::string& duuid,
    std::string& content,
    sqlite::database &dataDb
){
    try{
        dataDb << "update notesData set dcontent=? where token=? and duuid=?;"
               << content << token << duuid;
        return true;
    } catch(std::exception& e){
        std::cout  << "/notesHandler/notesHandler.cpp(saveNote) : " << e.what() << std::endl;
        return false;
    }
}

std::string viewNote(
    std::string &token,
    std::string &duuid,
    sqlite::database &dataDb
){
    try{
        std::string contents;
        dataDb << "select dcontent from notesData where token = ? and duuid = ?;"
               << token << duuid >> contents;
        return contents;
    } catch(std::exception& e){
        std::cout<< "/notesHandler/notesHandler.cpp(viewNote) : " << e.what() << std::endl;
        return "Failed to read notes";
    }

}
