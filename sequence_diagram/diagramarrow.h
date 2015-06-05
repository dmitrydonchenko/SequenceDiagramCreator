#ifndef DIAGRAMARROW_H
#define DIAGRAMARROW_H

#include <QtGui>

#include "diagramobject.h"

/*
Синхронное сообщение
*/

class DiagramArrow : public DiagramItem
{
public:
	//Конструкторы/Деструкторы
	DiagramArrow(QGraphicsItem *parent = 0);
	~DiagramArrow();

	//Обязательные функции, которые нужно переопределить
	//Функция, возвращающая приблизительную площадь отрисовываемую элементом
	virtual QRectF boundingRect() const;
	//Функция, реализующая отрисовку элемента
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//Функция, которая делает копию текущего элемента и возвращает указатель на нее
	virtual DiagramItem* getCopy() const;
	
	//Методы для работы с объектом
	void refreshValues();																							//Пересчитать значения длины, границы Drag and Drop и позиции
	virtual bool saveToFile(QDataStream &stream);																	//Сохранить элемент в файл
	virtual bool loadFromFile(QDataStream &stream, const vector<QGraphicsItem *> &items);							//Загрузить элемент из файла
	virtual QByteArray getInfoForBuffer();																			//Получить информацию для буфера
	virtual void setInfoFromBuffer(QList<QByteArray> list, vector<QGraphicsItem *> items, map<int, int> &new_id);	//Скопировать данные из буфера для текущего элемента

	//Модель сообщения
	QGraphicsItem *sourceItem;				//Откуда
	QGraphicsItem *targetItem;				//Куда
	bool isCreationMessageArrow;			//Если стрелка является сообщением создания

private:
	//Вспомогательные методы
	QGraphicsItem * getPtrToObject(int id, const vector<QGraphicsItem *> &items) const;		//Получить указатель на объект

protected:
};

#endif // DIAGRAMOBJECT_H
