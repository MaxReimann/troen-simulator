// STD
#include <iostream>
// Qt
#include <QApplication>
//troen
#include "MainWindow.h"
#include "troengame.h"

#define EXTERN
#include "globals.h"


#include <stdio.h>
#include <string.h>

//#define LOG_OUTPUT



using namespace troen;

/*! \file The main file is the entry point of the c++ application and sets up the Qt app (see mainwindow.cpp).*/

int main(int argc, char* argv[])
{


	int result = -1;

	// register meta types
	qRegisterMetaType<troen::GameConfig>("GameConfig");

	// setup application settings
	QApplication::setApplicationName("Troen");
	QApplication * application = new QApplication(argc, argv);

	troen::MainWindow * mainWindow = new troen::MainWindow();
	mainWindow->show();

#ifdef LOG_OUTPUT
	//std::ofstream log("Message.log");
	//std::streambuf* out = std::cout.rdbuf(log.rdbuf());
	//std::streambuf* err = std::cerr.rdbuf(log.rdbuf());
	freopen("Message.log", "w", stdout);
#endif

	result = application->exec();

	// Clean Up
	delete mainWindow;
	delete application;

	return result;
}