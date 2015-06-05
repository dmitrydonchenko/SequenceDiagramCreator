#include "diagramitem.h"

DiagramItem::DiagramItem(QGraphicsItem *parent)
	: QGraphicsItem(parent)
{
	//Устанавливаем начальный текст
	setTextComment("default");

	//Устанавливаем размеры по умолчанию
	width = 100;
	height = 50;

	//Устанавливаем флаг - объект можно выделить
	setFlag(ItemIsSelectable);

	//Инициализация общих свойств
	id = -1;
	item_type = DEFAULT_ITEM;
}

DiagramItem::~DiagramItem()
{
}

//Функция, реализующая отрисовку элемента
/*virtual*/ void DiagramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QString s = text_item_string;

	//Если длина сообщения больше чем необходимо
	if(s.size() > 10)
	{
		//Сокращаем строку до нужных размеров и ставим троеточие
		s = s.left(10);
		s += "...";
	}

	//Рисуем текст
	painter->drawText(text_pos_x, text_pos_y, s);
}

//Установить ширину объекта
void DiagramItem::setWidth(int w)
{
	//Устраняем плохие значения
	if(w <= 0) w = 1;

	//Задаем новое значение
	width = w;
}

//Установить высоту объекта
void DiagramItem::setHeight(int h)
{
	//Устраняем плохие значения
	if(h <= 0) h = 1;

	//Задаем новое значение
	height = h;
}

//Установить размеры объекта
void DiagramItem::setSize(int w, int h)
{
	//Задаем новые значения
	setWidth(w);
	setHeight(h);
}

//Получить ширину объекта
int DiagramItem::getWidth() const
{
	return width;
}

//Получить высоту объекта
int DiagramItem::getHeight() const
{
	return height;
}

//Функция, возвращающая точные границы элемента
/*virtual*/ QPainterPath DiagramItem::shape() const
{
	return QGraphicsItem::shape();
}

//Установить положение объекта
void DiagramItem::setPos(qreal x, qreal y)
{
	QGraphicsItem::setPos(x, y);
}

//Установить положение объекта по оси X
void DiagramItem::setPosX(qreal x)
{
	setPos(x, QGraphicsItem::pos().y());
}

//Установить положение объекта по оси Y
void DiagramItem::setPosY(qreal y)
{
	setPos(QGraphicsItem::pos().x(), y);
}

//Получить текстовый комментарий элемента
QString DiagramItem::getTextItemString() const
{
	return text_item_string;
}

//Установить текстовый комментарий
/*virtual*/ void DiagramItem::setTextComment(QString s)
{
	//Если строка пустая - оставляем предыдущую строку
	if(s == "")
		return;

	//Обновляем текстовый комментарий
	text_item_string = s;

	//Обновляем всплывающую подсказку
	this->setToolTip(s);

	//Устанавливаем позицию надписи
	updateTextCommentPos(s);
}

//Обновить позицию текстового комментария
/*virtual*/ void DiagramItem::updateTextCommentPos(QString s)
{
	QString str = s;

	//Если длина сообщения больше чем необходимо
	if(str.size() > 10)
	{
		//Сокращаем строку до нужных размеров и ставим троеточие
		str = s.left(10);
		str += "...";
	}
	//Вычисляем ширину и высоту текста
	int w = QFontMetrics(QGraphicsTextItem().font()).width(str);
	int h = QFontMetrics(QGraphicsTextItem().font()).height();

	//Перемещаем в нужную позицию
	text_pos_x = width / 2 - w / 2 - w / s.size() + 1;
	text_pos_y = height / 2 - h / 2;
}

//Задать высоту, на которой расположен объект
/*virtual*/ void DiagramItem::setVerticalSpace(int val)
{
	//Устраняем плохие значения
	if(val < 0) val = 0;

	//Задаем новое значение
	vertical_space = val;

	//Указываем новую позицию
	setPos(pos().x(), vertical_space);

	//Обновить
	update();
}

//Узнать на какой высоте находится элемент диаграммы
int DiagramItem::getVerticalSpace() const
{
	return vertical_space;
}

//Сохранить элемент в файл
/*virtual*/ bool DiagramItem::saveToFile(QDataStream &stream)
{
	stream << id;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << width;
	if(stream.status() != QDataStream::Ok)
		return false;
	
	stream << height;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << QGraphicsItem::x();
	if(stream.status() != QDataStream::Ok)
		return false;
	
	stream << QGraphicsItem::y();
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << vertical_space;
	if(stream.status() != QDataStream::Ok)
		return false;

	stream << text_item_string;
	if(stream.status() != QDataStream::Ok)
		return false;

	return true;
}

//Загрузить элемент из файла
/*virtual*/ bool DiagramItem::loadFromFile(QDataStream &stream)
{
	stream >> id;
	if(stream.status() != QDataStream::Ok)
		return false;
	
	stream >> width;
	if(stream.status() != QDataStream::Ok)
		return false;
	
	stream >> height;
	if(stream.status() != QDataStream::Ok)
		return false;

	setSize(width, height);

	qreal x, y;
	stream >> x;
	if(stream.status() != QDataStream::Ok)
		return false;
	stream >> y;
	if(stream.status() != QDataStream::Ok)
		return false;
	setPos(x, y);

	stream >> vertical_space;
	if(stream.status() != QDataStream::Ok)
		return false;
	setVerticalSpace(vertical_space);

	stream >> text_item_string;
	if(stream.status() != QDataStream::Ok)
		return false;
	setTextComment(text_item_string);

	return true;
}

//Сохранить тип элемента
bool DiagramItem::saveItemType(QDataStream &stream)
{
	stream << int(item_type);
	if(stream.status() != QDataStream::Ok)
		return false;
	return true;
}

//Загрузить тип элемента
bool DiagramItem::loadItemType(QDataStream &stream, EItemsType &type)
{
	int itype;
	stream >> itype;
	item_type = EItemsType(itype);
	type = item_type;
	if(stream.status() != QDataStream::Ok)
		return false;
	return true;
}

//Получить все параметры из другого элемента
void DiagramItem::setAllParamFromOtherItem(const DiagramItem * const other)
{
	id = other->id;
	item_type = other->item_type;

	width = other->width;
	height = other->height;
	vertical_space = other->vertical_space;

	text_pos_x = other->text_pos_x;
	text_pos_y = other->text_pos_y;
	text_item_string = other->text_item_string;

	setPos(other->x(), other->y());
}

//Получить информацию для буфера
/*virtual*/ QByteArray DiagramItem::getInfoForBuffer()
{
	QByteArray buf("");
	buf.append("<diagramitem_tag>\t");
	buf.append(QString::number(id) + "\t");
	buf.append(QString::number((int)item_type) + "\t");
	buf.append(QString::number(width) + "\t");
	buf.append(QString::number(height) + "\t");
	buf.append(QString::number(vertical_space) + "\t");
	buf.append(QString::number(text_pos_x) + "\t");
	buf.append(QString::number(text_pos_y) + "\t");
	buf.append(text_item_string + "\t");
	buf.append(QString::number((int)x()) + "\t");
	buf.append(QString::number((int)y()) + "\t");
	buf.append("</diagramitem_tag>");
	return buf;
}

//Скопировать данные из буфера для текущего элемента
/*virtual*/ void DiagramItem::setInfoFromBuffer(QList<QByteArray> list)
{
	QList<QByteArray>::iterator it = list.begin();
	it++;

	id = it->toInt();
	it++;

	item_type = EItemsType(it->toInt());
	it++;

	setWidth(it->toInt());
	it++;
	setHeight(it->toInt());
	it++;

	setVerticalSpace(it->toInt());
	it++;

	text_pos_x = it->toInt();
	it++;
	text_pos_y = it->toInt();
	it++;

	text_item_string = *it;
	it++;

	int xpos = it->toInt();
	it++;
	int ypos = it->toInt();
	it++;
	setPos(xpos, ypos);
}