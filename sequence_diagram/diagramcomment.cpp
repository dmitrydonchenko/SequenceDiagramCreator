#include "diagramcomment.h"

DiagramComment::DiagramComment(QGraphicsItem *parent)
	: DiagramItem(parent)
{
	//Устанавливаем начальный текст
	setTextComment("Comment");

	//Инициализация общих свойств
	item_type = COMMENT_ITEM;
}

DiagramComment::~DiagramComment()
{
}

//Функция, реализующая отрисовку элемента
void DiagramComment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	//Запоминаем какая сейчас кисть
	QPen current_pen = painter->pen();

	//Определяем область закрашивания
	QPainterPath fillArea;
	fillArea.moveTo(0, 0);
	fillArea.lineTo(width - 20, 0);
	fillArea.lineTo(width, height - 30);
	fillArea.lineTo(width, height);
	fillArea.lineTo(0, height);
	fillArea.lineTo(0, 0);

	//Закрашиваем фон
	painter->fillPath(fillArea, QBrush(QColor(255, 255, 255)));

	//Отрисовываем текст
	DiagramItem::paint(painter, option, widget);

	//Рисуем
	painter->drawLine(width - 20, 0, width, height - 30);

	//Рисуем рамку выделения - если объект выделен - иначе рисуем просто прямоугольник
	if(isSelected())
	{
		//Получаем границы выделения
		QPainterPath path = shape();

		// Ставим пунктирную кисть
		painter->setPen(QPen(Qt::DashLine));

		//Отрисовываем
		painter->drawPath(path);
	}
	else
	{
		painter->drawLine(0, 0, width - 20, 0);
		painter->drawLine(width, height - 30, width, height);
		painter->drawLine(width, height, 0, height);
		painter->drawLine(0, height, 0, 0);
	}

	//Возвращаем тип кисти
	painter->setPen(current_pen);
}

//Функция, возвращающая приблизительную площадь отрисовываемую элементом
QRectF DiagramComment::boundingRect() const
{
	return QRectF(0, 0, width, height);
}

//Функция, которая делает копию текущего элемента и возвращает указатель на нее
/*virtual*/ DiagramItem* DiagramComment::getCopy() const
{
	//Создаем новый элемент
	DiagramComment *new_comment = new DiagramComment();

	//Делаем указатель на него вида DiagramItem
	DiagramItem *new_item = static_cast<DiagramItem *>(new_comment);

	//Делаем указатель на текущий объект вида DiagramItem
	const DiagramItem * const cur_item = static_cast<const DiagramItem * const>(this);

	//Получаем параметры из родительского класса
	new_item->setAllParamFromOtherItem(cur_item);

	//Получаем параметры их текущего класса
	//Их нет :(

	return new_item;
}

//Получить информацию для буфера
/*virtual*/ QByteArray DiagramComment::getInfoForBuffer()
{
	QByteArray buf("");
	buf.append("<diagramcomment_tag>\t");
	buf.append(DiagramItem::getInfoForBuffer() + "\t");
	buf.append("</diagramcomment_tag>");
	return buf;
}

//Скопировать данные из буфера для текущего элемента
/*virtual*/ void DiagramComment::setInfoFromBuffer(QList<QByteArray> list)
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
}