#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QtGui>
#include <iostream>
#include <vector>

#include "dialogwindow.h"
#include "EItemsType.h"
#include "DiagramEditorParam.h"
#include "algo_sequence.h"

using namespace std;

/*
Класс элемента диаграммы (описывает основное поведение для всех объектов диаграммы)
*/

class DiagramItem : public QGraphicsItem
{
public:
	//Общие свойства
	int id;					//Идентификатор элемента диаграммы
	EItemsType item_type;	//Тип элемента диаграммы

	//Конструкторы/ Деструкторы
	DiagramItem(QGraphicsItem *parent = 0);
	~DiagramItem();

	//Обязательные функции, которые нужно переопределить
	//Функция, возвращающая прямоугольную область, которая будет считаться объектом (Bounding Box)
	virtual QRectF boundingRect() const = 0;
	//Функция, реализующая отрисовку элемента
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//Функция, которая делает копию текущего элемента и возвращает указатель на нее
	virtual DiagramItem* getCopy() const = 0;

	//Функции, которые желательно переопределить (если будет необходимость)
	//Функция, возвращающая точные границы элемента
	virtual QPainterPath shape() const;

	//Методы для работы с объектом
	void setAllParamFromOtherItem(const DiagramItem * const other);	//Получить все параметры из другого элемента
	void setWidth(int w);											//Установить ширину объекта
	void setHeight(int h);											//Установить высоту объекта
	void setSize(int w, int h);										//Установить размеры объекта
	void setPos(qreal x, qreal y);									//Установить положение объекта
	void setPosX(qreal x);											//Установить положение объекта по оси X
	void setPosY(qreal y);											//Установить положение объекта по оси Y
	int getWidth() const;											//Получить ширину объекта
	int getHeight() const;											//Получить высоту объекта
	int getVerticalSpace() const;									//Узнать на какой высоте находится элемент диаграммы
	QString getTextItemString() const;								//Получить текстовый комментарий элемента
	virtual QByteArray getInfoForBuffer();							//Получить информацию для буфера
	virtual void setInfoFromBuffer(QList<QByteArray> list);			//Скопировать данные из буфера для текущего элемента
	virtual void setTextComment(QString s);							//Установить текстовый комментарий
	virtual void setVerticalSpace(int val);							//Задать высоту, на которой расположен элемент диаграммы
	virtual bool saveToFile(QDataStream &stream);					//Сохранить элемент в файл
	virtual bool loadFromFile(QDataStream &stream);					//Загрузить элемент из файла
	bool saveItemType(QDataStream &stream);							//Сохранить тип элемента
	bool loadItemType(QDataStream &stream, EItemsType &type);		//Загрузить тип элемента

private:

protected:
	//Свойства объекта
	int width, height;					//Размеры
	int vertical_space;					//Высота (от верхнего края экрана), на котором расположен элемент диаграммы

	//Модель элемента диаграммы
	int text_pos_x, text_pos_y;				//Позиция текстового комментария
	QString text_item_string;				//Текстовый комментарий

	//Методы для работы с объектом
	virtual void updateTextCommentPos(QString s);		//Обновить позицию текстового комментария
};

#endif // DIAGRAMITEM_H
