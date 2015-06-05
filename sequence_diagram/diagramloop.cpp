#include "diagramloop.h"
#include <cmath>

DiagramLoop::DiagramLoop(QGraphicsItem *parent)
	: DiagramItem(parent)
{
	//Инициализация общих свойств
	item_type = LOOP_ITEM;
	sizeDetermined = false;
	loopTextCommentAreaWidth = 70;
	loopTextCommentAreaHeight = 20;
	setVerticalSpace(leftVertex.y());
}

DiagramLoop::~DiagramLoop()
{
}

//Функция, возвращающая приблизительную площадь отрисовываемую элементом
/*virtual*/ QRectF DiagramLoop::boundingRect() const
{
	return QRectF(0, 0, loopTextCommentAreaWidth, loopTextCommentAreaHeight);
}

//Функция, реализующая отрисовку элемента
/*virtual*/ void DiagramLoop::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	
	//Запоминаем какая сейчас кисть
	QPen current_pen = painter->pen();

	//Рисуем прямоугольник в зависимости от того - выделен объект или нет
	if(!isSelected())
		painter->drawRect(0, 0, width, height);

	painter->setPen(QPen(Qt::SolidLine));
	if(sizeDetermined)
	{
		//painter->drawRect(0, 0, loopTextCommentAreaWidth, loopTextCommentAreaHeight);

		QPainterPath fillArea;
		//Определяем область закрашивания
		fillArea.moveTo(0, 0);
		fillArea.lineTo(loopTextCommentAreaWidth, 0);
		fillArea.lineTo(loopTextCommentAreaWidth, loopTextCommentAreaHeight - 7);
		fillArea.lineTo(loopTextCommentAreaWidth - 7, loopTextCommentAreaHeight);
		fillArea.lineTo(0, loopTextCommentAreaHeight);
		fillArea.lineTo(0, 0);

		//Закрашиваем фон
		painter->fillPath(fillArea, QBrush(QColor(255, 255, 255)));

		painter->drawLine(0, 0, loopTextCommentAreaWidth, 0);
		painter->drawLine(loopTextCommentAreaWidth, 0, loopTextCommentAreaWidth, loopTextCommentAreaHeight - 7);
		painter->drawLine(loopTextCommentAreaWidth, loopTextCommentAreaHeight - 7, loopTextCommentAreaWidth - 7, loopTextCommentAreaHeight);
		painter->drawLine(loopTextCommentAreaWidth - 7, loopTextCommentAreaHeight, 0, loopTextCommentAreaHeight);
		painter->drawLine(0, loopTextCommentAreaHeight, 0, 0);

		fillArea.addRect(1, 1, loopTextCommentAreaWidth - 1, loopTextCommentAreaHeight - 1);
		
		QString s = text_item_string;

		//Если длина сообщения больше чем необходимо
		if(s.size() > 10)
		{
			//Сокращаем строку до нужных размеров и ставим троеточие
			s = s.left(10);
			s += "...";
		}
		painter->drawText(5, 12, "loop");
		painter->drawText(loopTextCommentAreaWidth + 5, 12, s);

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
	}
}

//Получить указатель на объект
QGraphicsItem * DiagramLoop::getPtrToObject(int id, const vector<QGraphicsItem *> &items) const
{
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->id == id)
			return items[i];
	}
	return NULL;
}

//Функция, возвращающая точные границы элемента
/*virtual*/ QPainterPath DiagramLoop::shape() const
{
	//Создаем новую границу
	QPainterPath path;

	//Отрисовываем границу (область выделения полностью описывает фигуру 
	path.moveTo(0, 0);
	path.lineTo(width, 0);
	path.lineTo(width, height);
	path.lineTo(0, height);
	path.lineTo(0, 0);
	return path;
}

//Функция, которая делает копию текущего элемента и возвращает указатель на нее
/*virtual*/ DiagramItem* DiagramLoop::getCopy() const
{
	//Создаем новый элемент
	DiagramLoop *new_loop = new DiagramLoop();

	//Делаем указатель на него вида DiagramItem
	DiagramItem *new_item = static_cast<DiagramItem *>(new_loop);

	//Делаем указатель на текущий объект вида DiagramItem
	const DiagramItem * const cur_item = static_cast<const DiagramItem * const>(this);

	//Получаем параметры из родительского класса
	new_item->setAllParamFromOtherItem(cur_item);

	//Получаем параметры их текущего класса
	new_loop->leftVertex = leftVertex;
	new_loop->rightVertex = rightVertex;
	new_loop->beginVertex = beginVertex;
	new_loop->loopTextCommentAreaWidth = loopTextCommentAreaWidth;
	new_loop->loopTextCommentAreaHeight = loopTextCommentAreaHeight;
	new_loop->sizeDetermined = sizeDetermined;

	return new_item;
}

//Получить вектор вложенных циклов
vector<DiagramLoop *> DiagramLoop::getLoopInVector(vector<QGraphicsItem *> items)
{
	vector<DiagramLoop *> ret;
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем все не циклы
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::LOOP_ITEM)
			continue;
		
		//Пропускаем не вложенные
		DiagramLoop *loop = static_cast<DiagramLoop *>(items[i]);
		if(!isStacked(loop))
			continue;

		//Остались вложенные циклы
		ret.push_back(loop);
	}
	return ret;
}

//Вложен ли цикл other в текущий
bool DiagramLoop::isStacked(DiagramLoop *other) const
{
	if(leftVertex.x() < other->leftVertex.x() && leftVertex.y() < other->leftVertex.y() && 
			rightVertex.x() > other->rightVertex.x() && rightVertex.y() > other->rightVertex.y())
			return true;
	return false;
}

//Передвинуть цикл
void DiagramLoop::move_loop(int dx, int dy, set<int> &used)
{
	for(int i = 0; i < stacked_loops.size(); i++)
	{
		if(used.count(stacked_loops[i]->id))
			continue;

		used.insert(stacked_loops[i]->id);

		//Получаем текущие координаты
		qreal scene_loop_pos_x = stacked_loops[i]->scenePos().x();
		qreal scene_loop_pos_y = stacked_loops[i]->scenePos().y();

		//Смещаем
		scene_loop_pos_x += dx;
		scene_loop_pos_y += dy;

		//Задаем новую пощицию
		stacked_loops[i]->setPos(scene_loop_pos_x, scene_loop_pos_y);
		stacked_loops[i]->leftVertex.setX(scene_loop_pos_x);
		stacked_loops[i]->leftVertex.setY(scene_loop_pos_y);
		stacked_loops[i]->rightVertex.setX(scene_loop_pos_x + stacked_loops[i]->getWidth());
		stacked_loops[i]->rightVertex.setY(scene_loop_pos_y + stacked_loops[i]->getHeight());
	}
}

//Сохранить элемент в файл
/*virtual*/ bool DiagramLoop::saveToFile(QDataStream &stream)
{
	DiagramItem::saveToFile(stream);

	stream << leftVertex;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << rightVertex;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << beginVertex;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << loopTextCommentAreaWidth;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << loopTextCommentAreaHeight;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << sizeDetermined;
	if(stream.status() != QDataStream::Ok)
		return false;

	return true;
}

//Загрузить элемент из файла
/*virtual*/ bool DiagramLoop::loadFromFile(QDataStream &stream)
{
	if(!DiagramItem::loadFromFile(stream))
		return false;

	stream >> leftVertex;
	setPos(leftVertex.x(), leftVertex.y());
	setVerticalSpace(leftVertex.y());
	if(stream.status() != QDataStream::Ok)
		return false;

	stream >> rightVertex;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream >> beginVertex;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream >> loopTextCommentAreaWidth;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream >> loopTextCommentAreaHeight;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream >> sizeDetermined;
	if(stream.status() != QDataStream::Ok)
		return false;

	return true;
}

//Получить информацию для буфера
/*virtual*/ QByteArray DiagramLoop::getInfoForBuffer()
{
	QByteArray buf("");
	buf.append("<diagramloop_tag>\t");
	buf.append(DiagramItem::getInfoForBuffer() + "\t");
	buf.append(QString::number(leftVertex.x()) + "\t");
	buf.append(QString::number(leftVertex.y()) + "\t");
	buf.append(QString::number(rightVertex.x()) + "\t");
	buf.append(QString::number(rightVertex.y()) + "\t");
	buf.append(QString::number(beginVertex.x()) + "\t");
	buf.append(QString::number(beginVertex.y()) + "\t");
	buf.append(QString::number(loopTextCommentAreaWidth) + "\t");
	buf.append(QString::number(loopTextCommentAreaHeight) + "\t");
	buf.append(QString::number(sizeDetermined) + "\t");
	buf.append("</diagramloop_tag>");
	return buf;
}

//Скопировать данные из буфера для текущего элемента
/*virtual*/ void DiagramLoop::setInfoFromBuffer(QList<QByteArray> list)
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

	leftVertex.setX(it->toInt());
	it++;

	leftVertex.setY(it->toInt());
	it++;

	rightVertex.setX(it->toInt());
	it++;

	rightVertex.setY(it->toInt());
	it++;

	beginVertex.setX(it->toInt());
	it++;

	beginVertex.setY(it->toInt());
	it++;

	loopTextCommentAreaWidth = it->toInt();
	it++;

	loopTextCommentAreaHeight = it->toInt();
	it++;

	sizeDetermined = it->toInt();
	it++;
}