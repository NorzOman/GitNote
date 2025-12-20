
// CROW_ROUTE(app,"/register")([](){});

#include <crow/app.h>
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/json.h>
#include <cstdlib>
#include <exception>
#include "notesHandler/notesHandler.cpp"
#include "userHandler/userHandler.h"
#include "databaseHandler/sqlite_modern_cpp.h"

int main(){

    sqlite::database userDb("database/usersCreds.db");
    sqlite::database dataDb("database/usersData.db");

    try {
        userDb << "create table if not exists usersCreds(id integer primary key autoincrement , username text , password text , token text)";
        dataDb << "create table if not exists notesData(id integer primary key autoincrement , token text , dname text , dcontent text default , duuid text)";
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(-1);
    }

    crow::SimpleApp app;

    CROW_ROUTE(app,"/")([](){
        return crow::response(200,"Health OK");
    });

    CROW_ROUTE(app,"/me/save")([&userDb,&dataDb](const crow::request& req){
        auto body = crow::json::load(req.body);
        try {
            std::string token = body["token"].s();

            // -> start from here . end note : 01:00 am

            if(verifyToken(token,userDb)){
                saveNote(std::string &token, std::string &duuid, std::string &content, sqlite::database &dataDb)
            }

        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            return crow::response(500,"Internal Service Error");
        }
    });

    CROW_ROUTE(app,"/me/create")([&userDb,&dataDb](const crow::request& req){
        auto body = crow::json::load(req.body);
        try {
            std::string token = body["token"].s();
            if(verifyToken(token,userDb)){
                std::string title = body["title"].s();
                std::string uuid = createNote(token,title,dataDb);
                return crow::response(200,uuid);
            } else {
                return crow::response(403,"Invalid token provided");
            }
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            return crow::response(500,"Internal Service Error");
        }
    });

    CROW_ROUTE(app,"/me")([&userDb,&dataDb](const crow::request& req){
        auto body = crow::json::load(req.body);
        try{
            std::string token = body["token"].s();
            userInfo u = userLookup(token,userDb,dataDb);
            crow::json::wvalue json;
            json["username"] = u.username;
            for (size_t i = 0; i < u.notes.size(); ++i) {
                json["notes"][i]["name"] = u.notes[i].first;
                json["notes"][i]["uuid"] = u.notes[i].second;
            }
            return crow::response(200,json.dump());
        } catch(std::exception& e){
            std::cout << e.what() << std::endl;
            return crow::response(500,"Internal Service Error");
        }
    });


    CROW_ROUTE(app,"/register")([&userDb](const crow::request& req){
        auto body = crow::json::load(req.body);
        try{
            std::string username = body["username"].s();
            std::string password = body["password"].s();
            std::string token = registerUser(username,password,userDb);
            if(token == "-1") return crow::response(401,"Failed to register");
            else return crow::response(200,token);
        } catch(std::exception& e){
            std::cout << e.what() << std::endl;
            return crow::response(500,"Internal Service Error");
        }
    });

    CROW_ROUTE(app,"/login")([&userDb](const crow::request& req){
        auto body = crow::json::load(req.body);
        try{
            std::string username = body["username"].s();
            std::string password = body["password"].s();
            std::string token = loginUser(username,password,userDb);
            if(token == "-1") return crow::response(401,"Invalid username & password pair was provided");
            else return crow::response(200,token);
        } catch(std::exception& e){
            std::cout << e.what() << std::endl;
            return crow::response(500,"Internal Service Error");
        }
    });

    app.port(8080).run();
}
