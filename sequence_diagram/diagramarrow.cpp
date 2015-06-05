#include "diagramarrow.h"
#include <cmath>

DiagramArrow::DiagramArrow(QGraphicsItem *parent)
	: DiagramItem(parent)
{
	//������ 7 ��������
	height = 7;
	
	//������������� ��������� �����
	setTextComment("Comment");

	//������ ������������ ������� �� ���������
	setVerticalSpace(150);
 
	//��������� ��������
	sourceItem = NULL;
	targetItem = NULL;

	//������������� ����� �������
	item_type = MESSAGE_ITEM;

	//���������� - �������� �� ��������� ���������� �������� (�� ��������� - ���)
	isCreationMessageArrow = false;
}

DiagramArrow::~DiagramArrow()
{
}

//�������, ������������ ��������������� ������� �������������� ���������
/*virtual*/ QRectF DiagramArrow::boundingRect() const
{
	return QRectF(0, 0, width, height);
}

//�������, ����������� ��������� ��������
/*virtual*/ void DiagramArrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	//������������ �����
	updateTextCommentPos(text_item_string);
	DiagramItem::paint(painter, option, widget);

	//���� ��� �� ������� �������, ������� ������� ���������� - �� ������������
	if(sourceItem == NULL || targetItem == NULL)
		return;

	//���������� ����� ������ �����
	QPen current_pen = painter->pen();

	//������ ����� ��������� ������� - ���� ������ �������
	if(isSelected())
	{
		//������ ���������� �����
		painter->setPen(QPen(Qt::DashLine));

		//�������� ������� ���������
		QPainterPath path = shape();

		//������������
		painter->drawPath(path);

		//���������� ��� �����
		painter->setPen(current_pen);
	}

	//���������� �������
	DiagramObject *source = static_cast<DiagramObject *>(sourceItem);
	DiagramObject *target = static_cast<DiagramObject *>(targetItem);

	//���� ������� �������� ���������� ��������
	if(this->isCreationMessageArrow)
	{
		//������� ������ ������ � �������
		int h = getVerticalSpace() - target->getHeight() / 2 + getHeight() / 2;
		target->setVerticalSpace(h);
		target->setPosY(h);
	}

	//���������� ���������� �� ��� X ����� �����
	qreal source_life_x = source->scenePos().x() + source->getWidth() / 2;
	qreal target_life_x = target->scenePos().x() + source->getWidth() / 2;
	if(this->isCreationMessageArrow)
	{
		if(source_life_x <= target_life_x)
			target_life_x = target->scenePos().x();
		else
			target_life_x = target->scenePos().x() + target->getWidth();
	}

	//���������� ������� �������
	width = abs(source_life_x - target_life_x);

	//���������� �����������
	if(source_life_x <= target_life_x)
	{
		//��������� �������
		setPosX(source_life_x);

		//������ �������
		QPolygonF trianglePolygon;
		trianglePolygon << QPointF(width, 4)   << 
						QPointF(width - 30, 0) << 
						QPointF(width - 30, 7);
		painter->setBrush(Qt::black);
		painter->drawPolygon(trianglePolygon);
	}
	else
	{
		//��������� �������
		setPosX(target_life_x);

		//������ �������
		QPolygonF trianglePolygon;
		trianglePolygon << QPointF(0, 4)   << 
						QPointF(30, 0) << 
						QPointF(30, 7);
		painter->setBrush(Qt::black);
		painter->drawPolygon(trianglePolygon);
	}

	//������ �����
	painter->drawLine(0, 4, width, 4);
}

//����������� �������� �����, ������� Drag and Drop � �������
void DiagramArrow::refreshValues()
{
	//���� ��� �� ������� �������, ������� ������� ���������� - ������ �� ������
	if(sourceItem == NULL || targetItem == NULL)
		return;

	//���������� �������
	DiagramObject *source = static_cast<DiagramObject *>(sourceItem);
	DiagramObject *target = static_cast<DiagramObject *>(targetItem);

	//���������� ���������� �� ��� X ����� �����
	qreal source_life_x = source->scenePos().x() + source->getWidth() / 2;
	qreal target_life_x = target->scenePos().x() + target->getWidth() / 2;

	//���������� ������� �������
	width = abs(source_life_x - target_life_x);

	//���������� �����������
	if(source_life_x <= target_life_x)
	{
		//��������� �������
		setPosX(source_life_x);

	}
	else
	{
		//��������� �������
		setPosX(target_life_x);
	}

	//��������� �����
	if(this->isCreationMessageArrow)
	{
		int h = getVerticalSpace() - target->getHeight() / 2 + getHeight() / 2;

		//������������ ������������ ������
		target->setVerticalSpace(h);
	}
	else
	{
		int h = max(target->getVerticalSpace() + target->getHeight(), source->getVerticalSpace() + source->getHeight());
		h = max(getVerticalSpace(), h);

		setVerticalSpace(h);
	}

	//��������� �������
	update();
}

//��������� ������� � ����
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

//��������� ������� �� �����
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

//�������� ��������� �� ������
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

//�������, ������� ������ ����� �������� �������� � ���������� ��������� �� ���
/*virtual*/ DiagramItem* DiagramArrow::getCopy() const
{
	//������� ����� �������
	DiagramArrow *new_arrow = new DiagramArrow();

	//������ ��������� �� ���� ���� DiagramItem
	DiagramItem *new_item = static_cast<DiagramItem *>(new_arrow);

	//������ ��������� �� ������� ������ ���� DiagramItem
	const DiagramItem * const cur_item = static_cast<const DiagramItem * const>(this);

	//�������� ��������� �� ������������� ������
	new_item->setAllParamFromOtherItem(cur_item);

	//�������� ��������� �� �������� ������
	new_arrow->sourceItem = sourceItem;
	new_arrow->targetItem = targetItem;
	new_arrow->isCreationMessageArrow = isCreationMessageArrow;

	return new_item;
}

//�������� ���������� ��� ������
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

//����������� ������ �� ������ ��� �������� ��������
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