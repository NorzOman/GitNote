#include "loginHandler.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

loginHandler::loginHandler(QWidget *parent)
    : QWidget{parent}
{
    resize(800,600);

    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel("Login");
    title->setAlignment(Qt::AlignCenter);

    auto* username = new QLineEdit;
    username->setPlaceholderText("Enter Username");

    auto* password = new QLineEdit;
    password->setPlaceholderText("Enter Password");
    password->setEchoMode(QLineEdit::Password);

    auto* loginBtn = new QPushButton("Login");

    layout->addWidget(title);
    layout->addWidget(username);
    layout->addWidget(password);
    layout->addWidget(loginBtn);

    connect(loginBtn, &QPushButton::clicked,this,[this,username,password](){
        qDebug() << "Attempt to login :- ";
        qDebug() << "username" << username->text();
        qDebug() << "password" << password->text();
    });
}
