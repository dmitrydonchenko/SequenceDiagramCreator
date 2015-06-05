#include "diagramobject.h"

DiagramObject::DiagramObject(QGraphicsItem *parent)
	: DiagramItem(parent)
{	
	//Устанавливаем начальный текст
	setTextComment("Object");

	//Определяем длину линии жизни
	life_line_len = life_line_len_default;

	//Определяем высоту расположения объекта по умолчанию
	vertical_space = vertical_space_default;

	//Корректируем высоту расположения объекта
	setPos(pos().x(), vertical_space);

	//Линия жизни не остановлена
	is_life_line_stop = false;

	//Инициализация общих свойств
	item_type = OBJECT_ITEM;
}

DiagramObject::~DiagramObject()
{
}

//Функция, возвращающая приблизительную площадь отрисовываемую элементом
/*virtual*/ QRectF DiagramObject::boundingRect() const
{
	return QRectF(0, 0, width, height + life_line_len);
}

//Функция, реализующая отрисовку элемента
/*virtual*/ void DiagramObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	//Запоминаем какая сейчас кисть
	QPen current_pen = painter->pen();

	//Определяем область закрашивания
	QPainterPath fillArea;
	fillArea.addRect(0, 0, width, height);

	//Закрашиваем фон
	painter->fillPath(fillArea, QBrush(QColor(255, 255, 255)));

	//Отрисовываем текст
	DiagramItem::paint(painter, option, widget);

	//Рисуем прямоугольник в зависимости от того - выделен объект или нет
	if(isSelected())
		painter->drawLine(width / 2 - 4, height, width / 2 + 4, height);
	else
		painter->drawRect(0, 0, width, height);

	//Ставим пунктирную кисть
	painter->setPen(QPen(Qt::DashLine));

	//Рисуем рамку выделения объекта - если объект выделен
	if(isSelected())
	{
		//Получаем границы выделения
		QPainterPath path = shape();

		//Отрисовываем
		painter->drawPath(path);
	}

	//Рисуем линию жизни
	painter->drawLine(width / 2, height, width / 2, height + life_line_len);

	//Возвращаем тип кисти
	painter->setPen(current_pen);

	//Рисуем активности
	for(int i = 0; i < activity.size(); i++)
	{
		painter->drawRect(width / 2 - 4, activity[i].first, 8, activity[i].second - activity[i].first);
		painter->fillRect(width / 2 - 4 + 1, activity[i].first + 1, 8 - 2, activity[i].second - activity[i].first - 2, Qt::white);
	}

	//Рисуем остановку линии жизни если есть
	if(isLifeLineStop())
	{
		painter->drawLine(width / 2 - 4, height + life_line_len - 4, width / 2 + 4, height + life_line_len + 4);
		painter->drawLine(width / 2 - 4, height + life_line_len + 4, width / 2 + 4, height + life_line_len - 4);
	}
}

//Задать длину линии жизни
void DiagramObject::setLifeLineLen(int val)
{
	//Устраняем плохие значения
	if(val < 0) val = 0;

	//Задаем новое значение
	life_line_len = val;
}

//Задать высоту, на которой расположен объект
/*virtual*/ void DiagramObject::setVerticalSpace(int val)
{
	//Метод по умолчанию
	DiagramItem::setVerticalSpace(val);

	//Корректируем высоту расположения объекта
	setPos(pos().x(), vertical_space);
}

//Функция, возвращающая точные границы элемента
/*virtual*/ QPainterPath DiagramObject::shape() const
{
	//Создаем новую границу
	QPainterPath path;

	//Отрисовываем границу (область выделения полностью описывает фигуру (кроме линии жизни - ширина выделения 8 пикселей)
	path.moveTo(0, 0);
	path.lineTo(width, 0);
	path.lineTo(width, height);
	path.lineTo(width / 2 + 4, height);
	path.lineTo(width / 2 + 4, height + life_line_len);
	path.lineTo(width / 2 - 4, height + life_line_len);
	path.lineTo(width / 2 - 4, height);
	path.lineTo(0, height);
	path.lineTo(0, 0);

	return path;
}

//Узнать длину линии жизни
int DiagramObject::getLifeLineLen() const
{
	return life_line_len;
}

//Установить активность
void DiagramObject::setActivity(vector<pair<int, int> > a)
{
	activity = a;
}

//Изменить состояние остановки линии жизни
void DiagramObject::changeStateLifeLineStop(int last_arrow_h)
{
	is_life_line_stop = !is_life_line_stop;
	
	//Если остановили
	if(is_life_line_stop)
		setLifeLineLen(last_arrow_h - height + life_line_len_default);
}

//Узнать, остановлена ли линия жизни
bool DiagramObject::isLifeLineStop() const
{
	return is_life_line_stop;
}

//Сохранить элемент в файл
/*virtual*/ bool DiagramObject::saveToFile(QDataStream &stream)
{
	DiagramItem::saveToFile(stream);

	stream << life_line_len;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << is_life_line_stop;
	if(stream.status() != QDataStream::Ok)
		return false;

	return true;
}

//Загрузить элемент из файла
/*virtual*/ bool DiagramObject::loadFromFile(QDataStream &stream)
{
	if(!DiagramItem::loadFromFile(stream))
		return false;

	stream >> life_line_len;
	if(stream.status() != QDataStream::Ok)
		return false;
	if(life_line_len < 0)
		return false;
	setLifeLineLen(life_line_len);

	stream >> is_life_line_stop;
	if(stream.status() != QDataStream::Ok)
		return false;

	return true;
}

//Функция, которая делает копию текущего элемента и возвращает указатель на нее
/*virtual*/ DiagramItem* DiagramObject::getCopy() const
{
	//Создаем новый элемент
	DiagramObject *new_object = new DiagramObject();

	//Делаем указатель на него вида DiagramItem
	DiagramItem *new_item = static_cast<DiagramItem *>(new_object);

	//Делаем указатель на текущий объект вида DiagramItem
	const DiagramItem * const cur_item = static_cast<const DiagramItem * const>(this);

	//Получаем параметры из родительского класса
	new_item->setAllParamFromOtherItem(cur_item);

	//Получаем параметры их текущего класса
	new_object->life_line_len = life_line_len;
	new_object->is_life_line_stop = is_life_line_stop;

	return new_item;
}

//Получить информацию для буфера
/*virtual*/ QByteArray DiagramObject::getInfoForBuffer()
{
	QByteArray buf("");
	buf.append("<diagramobject_tag>\t");
	buf.append(DiagramItem::getInfoForBuffer() + "\t");
	buf.append(QString::number(life_line_len) + "\t");
	buf.append(QString::number(is_life_line_stop) + "\t");
	buf.append("</diagramobject_tag>");
	return buf;
}

//Скопировать данные из буфера для текущего элемента
/*virtual*/ void DiagramObject::setInfoFromBuffer(QList<QByteArray> list)
{
	QList<QByteArray>::iterator it = list.begin();

	QList<QByteArray> item_list;
	do
	{
		it++;
		item_list.append(*it);
	}
	while(*it != "</diagramitem_tag>");
	DiagramItem::setInfoFromBuffer(item_list);
	it++;

	setLifeLineLen(it->toInt());
	it++;

	is_life_line_stop = it->toInt();
	it++;
}