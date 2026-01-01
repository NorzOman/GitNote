

#include "dashboardHandler.h"
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>

dashboardHandler::dashboardHandler(QWidget* parent) : QMainWindow(parent){
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* l = new QLabel("Hello World");
    layout->addWidget(l);
}
