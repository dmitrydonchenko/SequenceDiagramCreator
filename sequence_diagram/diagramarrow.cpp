#include "diagramarrow.h"
#include <cmath>

DiagramArrow::DiagramArrow(QGraphicsItem *parent)
	: DiagramItem(parent)
{
	//Высота 7 пикселей
	height = 7;
	
	//Устанавливаем начальный текст
	setTextComment("Comment");

	//Высота расположения стрелки по умолчанию
	setVerticalSpace(150);
 
	//Указатели занулены
	sourceItem = NULL;
	targetItem = NULL;

	//Инициализация общих свойств
	item_type = MESSAGE_ITEM;

	//Определяем - является ли сообщение сообщением создания (по умолчанию - нет)
	isCreationMessageArrow = false;
}

DiagramArrow::~DiagramArrow()
{
}

//Функция, возвращающая приблизительную площадь отрисовываемую элементом
/*virtual*/ QRectF DiagramArrow::boundingRect() const
{
	return QRectF(0, 0, width, height);
}

//Функция, реализующая отрисовку элемента
/*virtual*/ void DiagramArrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	//Отрисовываем текст
	updateTextCommentPos(text_item_string);
	DiagramItem::paint(painter, option, widget);

	//Если еще не указаны объекты, которые связаны сообщением - не отрисовывать
	if(sourceItem == NULL || targetItem == NULL)
		return;

	//Запоминаем какая сейчас кисть
	QPen current_pen = painter->pen();

	//Рисуем рамку выделения объекта - если объект выделен
	if(isSelected())
	{
		//Ставим пунктирную кисть
		painter->setPen(QPen(Qt::DashLine));

		//Получаем границы выделения
		QPainterPath path = shape();

		//Отрисовываем
		painter->drawPath(path);

		//Возвращаем тип кисти
		painter->setPen(current_pen);
	}

	//Определяем объекты
	DiagramObject *source = static_cast<DiagramObject *>(sourceItem);
	DiagramObject *target = static_cast<DiagramObject *>(targetItem);

	//Если стрелка является сообщением создания
	if(this->isCreationMessageArrow)
	{
		//Смещаем второй объект к стрелке
		int h = getVerticalSpace() - target->getHeight() / 2 + getHeight() / 2;
		target->setVerticalSpace(h);
		target->setPosY(h);
	}

	//Определяем координаты на оси X линий жизни
	qreal source_life_x = source->scenePos().x() + source->getWidth() / 2;
	qreal target_life_x = target->scenePos().x() + source->getWidth() / 2;
	if(this->isCreationMessageArrow)
	{
		if(source_life_x <= target_life_x)
			target_life_x = target->scenePos().x();
		else
			target_life_x = target->scenePos().x() + target->getWidth();
	}

	//Определяем размеры стрелки
	width = abs(source_life_x - target_life_x);

	//Определяем направление
	if(source_life_x <= target_life_x)
	{
		//Обновляем позицию
		setPosX(source_life_x);

		//Рисуем стрелку
		QPolygonF trianglePolygon;
		trianglePolygon << QPointF(width, 4)   << 
						QPointF(width - 30, 0) << 
						QPointF(width - 30, 7);
		painter->setBrush(Qt::black);
		painter->drawPolygon(trianglePolygon);
	}
	else
	{
		//Обновляем позицию
		setPosX(target_life_x);

		//Рисуем стрелку
		QPolygonF trianglePolygon;
		trianglePolygon << QPointF(0, 4)   << 
						QPointF(30, 0) << 
						QPointF(30, 7);
		painter->setBrush(Qt::black);
		painter->drawPolygon(trianglePolygon);
	}

	//Рисуем линию
	painter->drawLine(0, 4, width, 4);
}

//Пересчитать значения длины, границы Drag and Drop и позиции
void DiagramArrow::refreshValues()
{
	//Если еще не указаны объекты, которые связаны сообщением - ничего не делать
	if(sourceItem == NULL || targetItem == NULL)
		return;

	//Определяем объекты
	DiagramObject *source = static_cast<DiagramObject *>(sourceItem);
	DiagramObject *target = static_cast<DiagramObject *>(targetItem);

	//Определяем координаты на оси X линий жизни
	qreal source_life_x = source->scenePos().x() + source->getWidth() / 2;
	qreal target_life_x = target->scenePos().x() + target->getWidth() / 2;

	//Определяем размеры стрелки
	width = abs(source_life_x - target_life_x);

	//Определяем направление
	if(source_life_x <= target_life_x)
	{
		//Обновляем позицию
		setPosX(source_life_x);

	}
	else
	{
		//Обновляем позицию
		setPosX(target_life_x);
	}

	//Коррекция высот
	if(this->isCreationMessageArrow)
	{
		int h = getVerticalSpace() - target->getHeight() / 2 + getHeight() / 2;

		//Корректируем вертикальную высоту
		target->setVerticalSpace(h);
	}
	else
	{
		int h = max(target->getVerticalSpace() + target->getHeight(), source->getVerticalSpace() + source->getHeight());
		h = max(getVerticalSpace(), h);

		setVerticalSpace(h);
	}

	//Обновляем стрелку
	update();
}

//Сохранить элемент в файл
/*virtual*/ bool DiagramArrow::saveToFile(QDataStream &stream)
{
	if(!DiagramItem::saveToFile(stream))
		return false;

	stream << isCreationMessageArrow;
	if(stream.status() != QDataStream::Ok)
		return false;
	
	DiagramItem *source = static_cast<DiagramItem *>(sourceItem);
	DiagramItem *target = static_cast<DiagramItem *>(targetItem);
	
	stream << source->id;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << target->id;
	if(stream.status() != QDataStream::Ok)
		return false;

	return true;
}

//Загрузить элемент из файла
/*virtual*/ bool DiagramArrow::loadFromFile(QDataStream &stream, const vector<QGraphicsItem *> &items)
{
	if(!DiagramItem::loadFromFile(stream))
		return false;

	stream >> isCreationMessageArrow;
	if(stream.status() != QDataStream::Ok)
		return false;

	int source_id, target_id;

	stream >> source_id;
	if(stream.status() != QDataStream::Ok)
		return false;
	
	stream >> target_id;
	if(stream.status() != QDataStream::Ok)
		return false;

	QGraphicsItem *source_ptr = getPtrToObject(source_id, items);
	QGraphicsItem *target_ptr = getPtrToObject(target_id, items);

	if(source_ptr == NULL || target_ptr == NULL)
		return false;

	sourceItem = source_ptr;
	targetItem = target_ptr;

	return true;
}

//Получить указатель на объект
QGraphicsItem * DiagramArrow::getPtrToObject(int id, const vector<QGraphicsItem *> &items) const
{
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->id == id)
			return items[i];
	}
	return NULL;
}

//Функция, которая делает копию текущего элемента и возвращает указатель на нее
/*virtual*/ DiagramItem* DiagramArrow::getCopy() const
{
	//Создаем новый элемент
	DiagramArrow *new_arrow = new DiagramArrow();

	//Делаем указатель на него вида DiagramItem
	DiagramItem *new_item = static_cast<DiagramItem *>(new_arrow);

	//Делаем указатель на текущий объект вида DiagramItem
	const DiagramItem * const cur_item = static_cast<const DiagramItem * const>(this);

	//Получаем параметры из родительского класса
	new_item->setAllParamFromOtherItem(cur_item);

	//Получаем параметры их текущего класса
	new_arrow->sourceItem = sourceItem;
	new_arrow->targetItem = targetItem;
	new_arrow->isCreationMessageArrow = isCreationMessageArrow;

	return new_item;
}

//Получить информацию для буфера
/*virtual*/ QByteArray DiagramArrow::getInfoForBuffer()
{
	QByteArray buf("");
	buf.append("<diagramarrow_tag>\t");
	buf.append(DiagramItem::getInfoForBuffer() + "\t");
	buf.append(QString::number(isCreationMessageArrow) + "\t");
	buf.append(QString::number(((DiagramItem *)(sourceItem))->id) + "\t");
	buf.append(QString::number(((DiagramItem *)(targetItem))->id) + "\t");
	buf.append("</diagramarrow_tag>");
	return buf;
}

//Скопировать данные из буфера для текущего элемента
/*virtual*/ void DiagramArrow::setInfoFromBuffer(QList<QByteArray> list, vector<QGraphicsItem *> items, map<int, int> &new_id)
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

	isCreationMessageArrow = it->toInt();
	it++;

	int source_id = it->toInt();
	it++;

	int target_id = it->toInt();
	it++;

	int new_source_id = new_id[source_id];
	int new_target_id = new_id[target_id];
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->id == new_source_id)
			sourceItem = items[i];
		if(it->id == new_target_id)
			targetItem = items[i];
	}
}