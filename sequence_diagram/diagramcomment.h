#ifndef DIAGRAMCOMMENT_H
#define DIAGRAMCOMMENT_H

#include "diagramitem.h"

/*
Свободный комментарий
*/

class DiagramComment : public DiagramItem
{
public:
	// Конструкторы/деструкторы
	DiagramComment(QGraphicsItem *parent = 0);
	~DiagramComment();

	//Функция, возвращающая приблизительную площадь отрисовываемую элементом
	virtual QRectF boundingRect() const;
	//Функция, реализующая отрисовку элемента
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//Функция, которая делает копию текущего элемента и возвращает указатель на нее
	virtual DiagramItem* getCopy() const;

	//Методы для работы с элементом
	virtual QByteArray getInfoForBuffer();							//Получить информацию для буфера
	virtual void setInfoFromBuffer(QList<QByteArray> list);			//Скопировать данные из буфера для текущего элемента
};

#endif // DIAGRAMCOMMENT_H