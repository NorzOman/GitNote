
// -- libs --
#include <crow/app.h>
#include <crow/common.h>
#include <crow/http_request.h>
#include <crow/http_response.h>
#include <crow/json.h>
#include <crow/middleware.h>
#include <cstdlib>
#include <exception>

// -- custom libs --
#include "notesHandler/notesHandler.h"
#include "userHandler/userHandler.h"
#include "databaseHandler/sqlite_modern_cpp.h"
#include "middleware/middleware.h"

// -- file macros --
#define _PORT 5000
#define _USER_CREDS_PATH "database/usersCreds.db"
#define _USER_SDATA_PATH "database/usersData.db"

// -- define error message macros --
#define _ERROR_EMPTY  "Missing fields or invalid data format was provided."
#define _ERROR_INTERNAL "An internal service error caused this to happen."
#define _ERROR_FORBIDDEN  "Access denied. Invalid credentials or insufficient permissions."
#define _ERROR_AUTH "Invalid authentication credentials were provided"
#define _ERROR_CONFLICT "Faled to perform operation , conflict occured possible data already exists"

// -- define error code macros --
#define _ERRORC_EMPTY 400
#define _ERRORC_INTERNAL 500
#define _ERRORC_FORBIDDEN 403
#define _ERRORC_AUTH 401
#define _ERRORC_CONFLICT 409


int main(){
    sqlite::database userDb(_USER_CREDS_PATH);
    sqlite::database dataDb(_USER_SDATA_PATH);

    try {
        userDb << "create table if not exists usersCreds(id integer primary key autoincrement , username text , password text , token text);";
        dataDb << "create table if not exists notesData(id integer primary key autoincrement , token text , dname text , dcontent text , duuid text);";
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        exit(-1);
    }

    crow::App<GitNoteMiddleware> app;

    CROW_ROUTE(app, "/")
    ([]{
        return crow::response(200,"Health OK");
    });

    // -- handle post only login request --
    CROW_ROUTE(app,"/login").methods(crow::HTTPMethod::POST)
    ([&userDb](const crow::request& req){
        crow::json::wvalue json;
        json["status"] = "success";

        auto body = crow::json::load(req.body);
        if(!body){
            json["data"]["error"] = _ERROR_EMPTY;
            return crow::response(_ERRORC_EMPTY , json);
        }

        try{
            std::string username = body["username"].s();
            std::string password = body["password"].s();
            if(username.empty() or password.empty()){
                json["data"]["error"] = _ERROR_EMPTY;
                return crow::response(_ERRORC_EMPTY , json);
            }
            std::string token = loginUser(username,password,userDb);
            if(token == "-1") {
                json["data"]["error"] = _ERROR_AUTH;
                return crow::response(_ERRORC_AUTH , json);
            }
            json["data"]["token"] = token;
            return crow::response(200,json);
        }

        catch(const std::exception& e){
            json["status"] = "failed";
            json["data"]["error"] = _ERROR_INTERNAL;
            std::cout << e.what() << std::endl;
            return crow::response(_ERRORC_INTERNAL , json);
        }
    });


    // -- handle post only regsiter request --
    CROW_ROUTE(app,"/register").methods(crow::HTTPMethod::POST)
    ([&userDb](const crow::request& req){
        crow::json::wvalue json;
        json["status"] = "success";

        auto body = crow::json::load(req.body);
        if(!body){
            json["data"]["error"] = _ERROR_EMPTY;
            return crow::response(_ERRORC_EMPTY , json);
        }

        try{
            std::string username = body["username"].s();
            std::string password = body["password"].s();
            if(username.empty() or password.empty()){
                json["data"]["error"] = _ERROR_EMPTY;
                return crow::response(_ERRORC_EMPTY , json);
            }

            std::string token = registerUser(username,password,userDb);
            if(token == "-1") {
                json["data"]["error"] = _ERROR_CONFLICT;
                return crow::response(_ERRORC_CONFLICT , json);
            }
            json["data"]["token"] = token;
            return crow::response(200,json);
        }

        catch(const std::exception& e){
            json["status"] = "failed";
            json["data"]["error"] = _ERROR_INTERNAL;
            std::cout << e.what() << std::endl;
            return crow::response(_ERRORC_INTERNAL , json);
        }
    });


    CROW_ROUTE(app,"/me").methods(crow::HTTPMethod::POST)
    ([&userDb,&dataDb](const crow::request& req){
        crow::json::wvalue json;
        json["status"] = "success";

        auto body = crow::json::load(req.body);
        if(!body){
            json["data"]["error"] = _ERROR_EMPTY;
            return crow::response(_ERRORC_EMPTY , json);
        }

        try{
            std::string token = body["token"].s();
            if(token.empty()){
                json["data"]["error"] = _ERROR_EMPTY;
                return crow::response(_ERRORC_EMPTY , json);
            }

            userInfo u = userLookup(token,userDb,dataDb);
            json["username"] = u.username;
            for (int i = 0; i < u.notes.size(); ++i) {
                json["notes"][i]["name"] = u.notes[i].first;
                json["notes"][i]["uuid"] = u.notes[i].second;
            }
            return crow::response(200,json);
        }
        catch(const std::exception& e){
            json["status"] = "failed";
            json["data"]["error"] = _ERROR_INTERNAL;
            std::cout << e.what() << std::endl;
            return crow::response(_ERRORC_INTERNAL , json);
        }
    });


    CROW_ROUTE(app,"/me/view").methods(crow::HTTPMethod::POST)
    ([&userDb,&dataDb](const crow::request& req){
        crow::json::wvalue json;
        json["status"] = "success";

        auto body = crow::json::load(req.body);
        if(!body){
            json["data"]["error"] = _ERROR_EMPTY;
            return crow::response(_ERRORC_EMPTY , json);
        }

        try {
            std::string token = body["token"].s();
            if(token.empty()){
                json["data"]["error"] = _ERROR_EMPTY;
                return crow::response(_ERRORC_EMPTY , json);
            }

            if(!verifyToken(token,userDb)){
                json["data"]["error"] = _ERROR_FORBIDDEN;
                return crow::response(_ERRORC_FORBIDDEN,_ERROR_FORBIDDEN);
            }

            std::string duuid = body["duuid"].s();
            std::string content = viewNote(token,duuid,dataDb);
            json["content"] = content;
            return crow::response(200,json);
        }
        catch (const std::exception& e) {
            json["status"] = "failed";
            json["data"]["error"] = _ERROR_INTERNAL;
            std::cout << e.what() << std::endl;
            return crow::response(_ERRORC_INTERNAL , json);
        }
    });

    CROW_ROUTE(app,"/me/save").methods(crow::HTTPMethod::POST)
    ([&userDb,&dataDb](const crow::request& req){
        crow::json::wvalue json;
        json["status"] = "success";

        auto body = crow::json::load(req.body);
        if(!body){
            json["data"]["error"] = _ERROR_EMPTY;
            return crow::response(_ERRORC_EMPTY , json);
        }

        try {
            std::string token = body["token"].s();
            if(token.empty()){
                json["data"]["error"] = _ERROR_EMPTY;
                return crow::response(_ERRORC_EMPTY , json);
            }

            if(verifyToken(token,userDb)){
                json["data"]["error"] = _ERROR_FORBIDDEN;
                return crow::response(_ERRORC_FORBIDDEN , json);
            }
            std::string duuid = body["duuid"].s();
            std::string content = body["content"].s();
            if(saveNote(token,duuid,content,dataDb)){
                json["data"]["save_status"] = "success";
                return crow::response(200,json);
            }else{
                json["data"]["error"] = "An unknown error caused the saving process to fail";
                return crow::response(_ERRORC_INTERNAL, json);
            }
        }
        catch (const std::exception& e) {
            json["status"] = "failed";
            json["data"]["error"] = _ERROR_INTERNAL;
            std::cout << e.what() << std::endl;
            return crow::response(_ERRORC_INTERNAL , json);
        }
    });

    CROW_ROUTE(app,"/me/create").methods(crow::HTTPMethod::POST)
    ([&userDb,&dataDb](const crow::request& req){
        crow::json::wvalue json;
        json["status"] = "success";

        auto body = crow::json::load(req.body);
        if(!body){
            json["data"]["error"] = _ERROR_EMPTY;
            return crow::response(_ERRORC_EMPTY , json);
        }

        try {
            std::string token = body["token"].s();
            std::string title = body["title"].s();
            if(token.empty() || title.empty()){
                json["data"]["error"] = _ERROR_EMPTY;
                return crow::response(_ERRORC_EMPTY , json);
            }

            if(verifyToken(token,userDb)){
                std::string uuid = createNote(token,title,dataDb);
                json["data"]["uuid"] = uuid;
                return crow::response(200,json);
            }
            else {
                json["data"]["error"] = _ERROR_FORBIDDEN;
                return crow::response(_ERRORC_FORBIDDEN , json);
            }
        }
        catch (const std::exception& e) {
            json["status"] = "failed";
            json["data"]["error"] = _ERROR_INTERNAL;
            std::cout << e.what() << std::endl;
            return crow::response(_ERRORC_INTERNAL , json);
        }
    });


    app.port(_PORT).run();
}
