#ifndef DIAGRAMLOOP_H
#define DIAGRAMLOOP_H

#include <QtGui>

#include "diagramobject.h"

/*
Синхронное сообщение
*/

class DiagramLoop : public DiagramItem
{
public:
	//Конструкторы/Деструкторы
	DiagramLoop(QGraphicsItem *parent = 0);
	~DiagramLoop();

	QPointF leftVertex;				//Левая верхняя вершина прямоугольника цикла
	QPointF rightVertex;			//Правая нижняя вершина
	QPointF beginVertex;			//Точка, с которой начинается отрисовка(вспомогательный параметр)

	int loopTextCommentAreaWidth;	//Длина области текстового комментария
	int loopTextCommentAreaHeight;	//Высота области текстового комментария

	bool sizeDetermined;			//Флаг, указывающий на то, что размер цикла уже определен

	DiagramLoop *parentLoop;

	vector<DiagramLoop *> stacked_loops;		//Вложенные циклы на данный момент

	//Передвинуть цикл
	void move_loop(int dx, int dy, set<int> &used);

	//Получить вектор вложенных циклов
	vector<DiagramLoop *> getLoopInVector(vector<QGraphicsItem *> items);

	//Вложен ли цикл other в текущий
	bool isStacked(DiagramLoop *other) const;


	//Обязательные функции, которые нужно переопределить
	//Функция, возвращающая приблизительную площадь отрисовываемую элементом
	virtual QRectF boundingRect() const;
	//Функция, реализующая отрисовку элемента
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//Функция, которая делает копию текущего элемента и возвращает указатель на нее
	virtual DiagramItem* getCopy() const;

	//void setTextComment(QString s);														//Установить текстовый комментарий
	virtual bool saveToFile(QDataStream &stream);											//Сохранить элемент в файл
	virtual bool loadFromFile(QDataStream &stream);											//Загрузить элемент из файла
	//Функции, которые желательно переопределить (если будет необходимость)
	//Функция, возвращающая точные границы элемента
	virtual QPainterPath shape() const;

	virtual QByteArray getInfoForBuffer();							//Получить информацию для буфера
	virtual void setInfoFromBuffer(QList<QByteArray> list);			//Скопировать данные из буфера для текущего элемента

private:
	//Методы для работы с объектом
	//void updateTextCommentPos(QString s);		//Обновить позицию текстового комментария

	//Вспомогательные методы
	QGraphicsItem * getPtrToObject(int id, const vector<QGraphicsItem *> &items) const;		//Получить указатель на объект
protected:
	//События
	//virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);				//Событие перемещения кнопки мыши
	//virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);		//Событие двойного клика мышью
};

#endif // DIAGRAMOBJECT_H
