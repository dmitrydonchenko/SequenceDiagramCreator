#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

#include "graphicscene.h"
#include "DiagramEditorParam.h"

//����� �������� ���� ���������
class mainWindow : public QMainWindow
{
	Q_OBJECT

public:
	//�����������/�����������
	mainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~mainWindow();

private:
	//GUI ��������� ���������
	Ui::mainWindowClass ui;

	//����������� �����
	GraphicScene *scene;

	//������ ��� ���������� ����������� �����
	QTimer *timer_lastdiagram;

	//���������� � ����� � ����������� � ��������� ����������
	QFile *file_lastdiagram;
	QString filename_lastdiagram;
	QMutex mutex_lastdiagram;

	//������ ��� ��������� ��������
	bool init_file_lastdiagram();					//������������� ����� ��������� ��������

public slots:
	//���������� ���������
	void addDiagramObject();							//�������� ������ ��������� ������������������
	void addDiagramComment();							//�������� ��������� �����������
	void addMessage();									//�������� ���������
	void addCreationMessage();							//�������� ��������� ��������
	void addStopLine();									//��������/������� ��������� ����� �����
	void addLoop();										//�������� ����

	//������ � ����������
	void removeItem();									//������� ���������� ������
	void cutAction();									//��������� ���������
	void copyAction();									//����������� ���������
	void pasteAction();									//������� ���������

	//������ �� ������
	void zoomIn();										//����������
	void zoomOut();										//��������
	void scaleScene();									//���������������
	
	
	//������ � ������� � ���������� � �����
	void createNewDiagram();							//������� ����� ���������
	void loadDiagramFromFile();							//��������� ��������� �� �����
	void loadDiagramFromLastDiagram();					//��������� ��������� �� ��������� ������
	void saveDiagramToFile();							//��������� ��������� � ����
	void saveAsDiagramToFile();							//��������� ��������� � ���� ���...
	void saveAsImage();									//��������� ��������� � ���� ��������
	void printAction();									//��������� ��������� �� ������

	//������ � ������� �����
	void refresh_lastdiagram();							//���������� ���� - ��������� ���������
	void refresh_file_lastdiagram(QString path);		//���������� ����� � ���������� �����������
};

#endif // MAINWINDOW_H
