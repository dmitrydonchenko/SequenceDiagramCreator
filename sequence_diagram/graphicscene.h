#ifndef GRAPHICSCENE_H
#define GRAPHICSCENE_H

#include <QtGui>
#include <iostream>
#include <vector>

#include "diagramobject.h"
#include "diagramcomment.h"
#include "diagramarrow.h"
#include "diagramloop.h"

#include "EStateType.h"
#include "DiagramEditorParam.h"
#include "algo_sequence.h"

using namespace std;

/*
Класс, описывающий сцену графического представления.
*/

class GraphicScene : public QGraphicsScene
{
	Q_OBJECT

public:
	//Конструкторы/Деструкторы
	GraphicScene(QObject *parent = 0);
	~GraphicScene();

	//Перегруженые методы для работы со сценой
	void addItem(QGraphicsItem *item);				//Добавление элемента на сцену

	//Методы для работы со сценой
	void setNewState(EStateType s);					//Установить новое состояние для сцены
	void removeSelectedItem();						//Удалить выделенный элемент
	void scaleScene(int p);							//Масштабирование сцены
	void changeStateOfLineEnd();					//Изменить состояние выделенного объекта (остановлена линия жизни или нет)
	void duplicateSelection();						//Сделать дубликат выделенных элементов
	void cutSelection();							//Вырезать выделенные элементы в буфер обмена
	void copySelection();							//Скопировать выделенные элементы в буфер обмена
	void pasteFromBuffer();							//Вставить элементы из буфера обмена
	void moveChildLoop(int dx, int dy, DiagramItem *it, set<int> &used);	

	//Методы для работы с файлами/сценой
	void newDiagram();								//Очистить диаграмму
	bool openDiagramFromFile(QString filename);		//Открыть диаграмму из файла
	bool saveDiagramToFile(QString filename);		//Сохранить диаграмму в файл

	//Вспомогательные методы
	QString getFilename() const;					//Получить текущее имя файла

private:
	//Файлы
	QString diagram_filename;			//Текущий путь к файлу диаграммы

	//Модель сцены
	int last_id;						//Последний использованный id
	vector<QGraphicsItem *> items;		//Элементы диаграммы, находящиеся на сцене
	EStateType current_state;			//Текущее состояние сцены

	//Для работы с буфером
	QMimeData *mimeData;

	//Вспомогательные средства
	DiagramArrow *new_arrow;			//Указатель на добавляему стрелку
	DiagramLoop *new_loop;				//Указатель на добавляемый цикл

	//Вспомогательные средства для определения состояния
	bool is_drag;						//Происходит ли сейчас действие перетаскивания объектов по сцене или нет
	bool is_ctrl;						//Зажата ли клавиша ctrl на данный момент или нет
	bool is_shift;						//Зажата ли клавиша shift на данный момент или нет
	bool is_duplicate;					//Было ли произведено дублирование за текущее перетаскивание или нет
	bool is_move;						//Было ли перетаскивание

	//Вспомогательные средства для Drag and Drop
	qreal mouse_drag_pos_x, mouse_drag_pos_y;	//Позиция курсора в момент начала перетаскивания объекта относительно сцены

	//Вспомогательные методы
	void update_lifelines();															//Обновить линии жизни
	void update_activity();																//Обновить активности
	void unselectAll();																	//Убрать выделение со всех объектов
	bool isCreationMessageCanBeCreated(DiagramObject *from, DiagramObject *to);			//Может ли стрелка создания быть создана
	int getMaxArrowHeight(DiagramObject *obj);											//Получить высоту последней стрелки, связанной с объектом
	
	//Реакции на состояния
	void mousePressEventDefault(QGraphicsSceneMouseEvent *event);								//Реакция на нажатие кнопки мыши в обычном состоянии
	void mousePressEventSelectPlaceForObject(QGraphicsSceneMouseEvent *event);					//Реакция на нажатие кнопки мыши в состоянии добавления объекта
	void mousePressEventSelectPlaceForComment(QGraphicsSceneMouseEvent *event);					//Реакция на нажатие кнопки мыши в состоянии добавления комментария
	void mousePressEventSelectMessageBegin(QGraphicsSceneMouseEvent *event);					//Реакция на нажатие кнопки мыши в состоянии добавления начала синх. сообщения
	void mousePressEventSelectMessageEnd(QGraphicsSceneMouseEvent *event);						//Реакция на нажатие кнопки мыши в состоянии добавления конца синх. сообщения
	void mousePressEventSelectCreationMessageBegin(QGraphicsSceneMouseEvent *event);			//Реакция на нажатие кнопки мыши в состоянии добавления начала сообщения создания
	void mousePressEventSelectCreationMessageEnd(QGraphicsSceneMouseEvent *event);				//Реакция на нажатие кнопки мыши в состоянии добавления конца сообщения создания
	void mousePressEventSelectLoopRectangleFirstVertex(QGraphicsSceneMouseEvent *event);		//Реакция на нажатие кнопки мыши в состоянии выбора первой вершины прямоугольника цикла
	void mouseMoveEventSelectLoopRectangleArea(QGraphicsSceneMouseEvent *event);				//Реакция на передвижение мыши в состоянии выделении прямоугольной области для цикла
	void mouseReleaseEventSelectLoopRectangleLastVertex(QGraphicsSceneMouseEvent *event);		//Реакция на нажатие кнопки мыши в состоянии выбора последней вершины прямоугольника цикла

	//Drag and Drop функции
	void move_object(int dx, int dy, DiagramItem *it, set<int> &used);					//Перемещение объекта
	void move_message(int dx, int dy, DiagramItem *it, set<int> &used);					//Перемещение синхронного сообщения
	void move_creation_message(int dx, int dy, DiagramItem *it, set<int> &used);		//Перемещение сообщения создания
	void move_comment(int dx, int dy, DiagramItem *it, set<int> &used);					//Перемещение свободного комментария
	void move_loop(int dx, int dy, DiagramItem *it, set<int> &used);					//Перемещение цикла			

protected:
	//События мыши
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);			//Нажата кнопка мыши
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);		//Отжата кнопка мыши
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);			//Перемещение мыши
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);	//Двойной клик мыши

	//События клавиатуры
	virtual void keyPressEvent(QKeyEvent *event);							//Нажали кнопку клавиатуры
	virtual void keyReleaseEvent(QKeyEvent *event);							//Отжали кнопку клавиатуры
};

#endif // GRAPHICSCENE_H
