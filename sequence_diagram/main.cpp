#include "mainwindow.h"
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec *tc=QTextCodec::codecForName("CP1251");
	QTextCodec::setCodecForCStrings(tc);
	mainWindow w;
	w.show();
	return a.exec();
}
