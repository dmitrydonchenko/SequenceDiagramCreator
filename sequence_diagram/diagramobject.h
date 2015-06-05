#ifndef DIAGRAMOBJECT_H
#define DIAGRAMOBJECT_H

#include "diagramitem.h"

/*
Объект (элемент диаграммы последовательности)
*/

class DiagramObject : public DiagramItem
{
public:
	//Конструкторы/Деструкторы
	DiagramObject(QGraphicsItem *parent = 0);
	~DiagramObject();

	//Обязательные функции, которые нужно переопределить
	//Функция, возвращающая приблизительную площадь отрисовываемую элементом
	virtual QRectF boundingRect() const;
	//Функция, реализующая отрисовку элемента
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//Функция, которая делает копию текущего элемента и возвращает указатель на нее
	virtual DiagramItem* getCopy() const;

	//Функции, которые желательно переопределить (если будет необходимость)
	//Функция, возвращающая точные границы элемента
	virtual QPainterPath shape() const;

	//Методы для работы с объектом
	int getLifeLineLen() const;										//Узнать длину линии жизни
	bool isLifeLineStop() const;									//Узнать, остановлена ли линия жизни
	void setLifeLineLen(int val);									//Задать длину линии жизни
	void setActivity(vector<pair<int, int> > a);					//Установить активность
	void changeStateLifeLineStop(int last_arrow_h);					//Изменить состояние остановки линии жизни
	virtual void setVerticalSpace(int val);							//Задать высоту, на которой расположен элемент диаграммы
	virtual bool saveToFile(QDataStream &stream);					//Сохранить элемент в файл
	virtual bool loadFromFile(QDataStream &stream);					//Загрузить элемент из файла
	virtual QByteArray getInfoForBuffer();							//Получить информацию для буфера
	virtual void setInfoFromBuffer(QList<QByteArray> list);			//Скопировать данные из буфера для текущего элемента
	
private:
	//Модель объекта
	vector<pair<int, int> > activity;			//Активность

	//Свойства объекта
	int life_line_len;							//Длина линии жизни
	bool is_life_line_stop;						//Флаг остановки линии жизни
};

#endif // DIAGRAMOBJECT_H
