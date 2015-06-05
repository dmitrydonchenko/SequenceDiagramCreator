#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

#include "graphicscene.h"
#include "DiagramEditorParam.h"

//Класс главного окна программы
class mainWindow : public QMainWindow
{
	Q_OBJECT

public:
	//Констукторы/Деструкторы
	mainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~mainWindow();

private:
	//GUI интерфейс программы
	Ui::mainWindowClass ui;

	//Графическая сцена
	GraphicScene *scene;

	//Таймер для обновления графической сцены
	QTimer *timer_lastdiagram;

	//Информация о файле с информацией о последних диаграммах
	QFile *file_lastdiagram;
	QString filename_lastdiagram;
	QMutex mutex_lastdiagram;

	//Методы для последних диаграмм
	bool init_file_lastdiagram();					//Инициализация файла последних диаграмм

public slots:
	//Добавление элементов
	void addDiagramObject();							//Добавить объект диаграммы последовательности
	void addDiagramComment();							//Добавить свободный комментарий
	void addMessage();									//Добавить сообщение
	void addCreationMessage();							//Добавить сообщение создания
	void addStopLine();									//Добавить/Удалить остановку линии жизни
	void addLoop();										//Добавить цикл

	//Работа с элементами
	void removeItem();									//Удалить выделенный объект
	void cutAction();									//Вырезание элементов
	void copyAction();									//Копирование элементов
	void pasteAction();									//Вставка элементов

	//Работа со сценой
	void zoomIn();										//Приблизить
	void zoomOut();										//Отдалить
	void scaleScene();									//Масштабирование
	
	
	//Работа с файлами и диаграммой в целом
	void createNewDiagram();							//Создать новую диаграмму
	void loadDiagramFromFile();							//Загрузить диаграмму из файла
	void loadDiagramFromLastDiagram();					//Загрузить диаграмму из последних файлов
	void saveDiagramToFile();							//Сохранить диаграмму в файл
	void saveAsDiagramToFile();							//Сохранить диаграмму в файл как...
	void saveAsImage();									//Сохранить диаграмму в виде картинки
	void printAction();									//Отправить диаграмму на печать

	//Работа с главным окном
	void refresh_lastdiagram();							//Обновление меню - последние диаграммы
	void refresh_file_lastdiagram(QString path);		//Обновление файла с последними диаграммами
};

#endif // MAINWINDOW_H
