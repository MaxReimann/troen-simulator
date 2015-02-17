#pragma once

#include <reflectionzeug/Object.h>
#include <scriptzeug/ScriptContext.h>

#include <QApplication>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QFile>
#include <QWidget>
#include <QMessageBox>
#include <QTextStream>

#include "abstractscript.h"
#include "../forwarddeclarations.h"

class ScriptWatcher : public QObject
{
	Q_OBJECT

public:
	/*! The ScriptWatcher is used to watch JavaScript-files which get automatically reloaded within their Scriptzeug-Context if they get updated.*/
	ScriptWatcher() : QObject(){

		m_watcher = new QFileSystemWatcher();
		
		//


		//QStringList directoryList = watcher->files();
		//Q_FOREACH(QString directory, directoryList)
		//	qDebug() << "Directory name" << directory << "\n";

		//directoryList = watcher->directories();
		//Q_FOREACH(QString directory, directoryList)
		//	qDebug() << "Directory name" << directory << "\n";


		//QObject::connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(onFolderChanged(QString)));
		//QObject::connect(m_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onFolderChanged(QString)));
	}

	~ScriptWatcher(){}

	void watchAndLoad(const QString& filePath, troen::AbstractScript *scriptObject, bool *changed)
	{
		m_changedFlag = changed;
		m_scriptContext = scriptObject;
		m_watchedFilePath = filePath;
		m_watcher->addPath(filePath);
		QObject::connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(onFolderChanged(QString)));
		
		onFolderChanged("");
	};
	std::string getContent()
	{
		QFile f(m_watchedFilePath);
		if (f.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream in(&f);
			content = in.readAll();
			f.close();
		}
		return content.toStdString();
	}

private:

	QString content;
	QString m_watchedFilePath;
	troen::AbstractScript* m_scriptContext;
	bool *m_changedFlag;
	QFileSystemWatcher *m_watcher;



public slots :
	void onFolderChanged(const QString& str)
	{
		std::cout << str.toStdString() << "was changed" << std::endl;
		//foreach(QString file, files) {

		QFile f(m_watchedFilePath);
		if (f.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream in(&f);
			content = in.readAll();
			std::string strcontent = content.toStdString();
			m_scriptContext->evaluate(strcontent);
			*m_changedFlag = true;

			f.close();
		}

		// }
	}
};