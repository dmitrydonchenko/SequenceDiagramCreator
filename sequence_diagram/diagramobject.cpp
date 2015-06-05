#include "diagramobject.h"

DiagramObject::DiagramObject(QGraphicsItem *parent)
	: DiagramItem(parent)
{	
	//������������� ��������� �����
	setTextComment("Object");

	//���������� ����� ����� �����
	life_line_len = life_line_len_default;

	//���������� ������ ������������ ������� �� ���������
	vertical_space = vertical_space_default;

	//������������ ������ ������������ �������
	setPos(pos().x(), vertical_space);

	//����� ����� �� �����������
	is_life_line_stop = false;

	//������������� ����� �������
	item_type = OBJECT_ITEM;
}

DiagramObject::~DiagramObject()
{
}

//�������, ������������ ��������������� ������� �������������� ���������
/*virtual*/ QRectF DiagramObject::boundingRect() const
{
	return QRectF(0, 0, width, height + life_line_len);
}

//�������, ����������� ��������� ��������
/*virtual*/ void DiagramObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	//���������� ����� ������ �����
	QPen current_pen = painter->pen();

	//���������� ������� ������������
	QPainterPath fillArea;
	fillArea.addRect(0, 0, width, height);

	//����������� ���
	painter->fillPath(fillArea, QBrush(QColor(255, 255, 255)));

	//������������ �����
	DiagramItem::paint(painter, option, widget);

	//������ ������������� � ����������� �� ���� - ������� ������ ��� ���
	if(isSelected())
		painter->drawLine(width / 2 - 4, height, width / 2 + 4, height);
	else
		painter->drawRect(0, 0, width, height);

	//������ ���������� �����
	painter->setPen(QPen(Qt::DashLine));

	//������ ����� ��������� ������� - ���� ������ �������
	if(isSelected())
	{
		//�������� ������� ���������
		QPainterPath path = shape();

		//������������
		painter->drawPath(path);
	}

	//������ ����� �����
	painter->drawLine(width / 2, height, width / 2, height + life_line_len);

	//���������� ��� �����
	painter->setPen(current_pen);

	//������ ����������
	for(int i = 0; i < activity.size(); i++)
	{
		painter->drawRect(width / 2 - 4, activity[i].first, 8, activity[i].second - activity[i].first);
		painter->fillRect(width / 2 - 4 + 1, activity[i].first + 1, 8 - 2, activity[i].second - activity[i].first - 2, Qt::white);
	}

	//������ ��������� ����� ����� ���� ����
	if(isLifeLineStop())
	{
		painter->drawLine(width / 2 - 4, height + life_line_len - 4, width / 2 + 4, height + life_line_len + 4);
		painter->drawLine(width / 2 - 4, height + life_line_len + 4, width / 2 + 4, height + life_line_len - 4);
	}
}

//������ ����� ����� �����
void DiagramObject::setLifeLineLen(int val)
{
	//��������� ������ ��������
	if(val < 0) val = 0;

	//������ ����� ��������
	life_line_len = val;
}

//������ ������, �� ������� ���������� ������
/*virtual*/ void DiagramObject::setVerticalSpace(int val)
{
	//����� �� ���������
	DiagramItem::setVerticalSpace(val);

	//������������ ������ ������������ �������
	setPos(pos().x(), vertical_space);
}

//�������, ������������ ������ ������� ��������
/*virtual*/ QPainterPath DiagramObject::shape() const
{
	//������� ����� �������
	QPainterPath path;

	//������������ ������� (������� ��������� ��������� ��������� ������ (����� ����� ����� - ������ ��������� 8 ��������)
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

//������ ����� ����� �����
int DiagramObject::getLifeLineLen() const
{
	return life_line_len;
}

//���������� ����������
void DiagramObject::setActivity(vector<pair<int, int> > a)
{
	activity = a;
}

//�������� ��������� ��������� ����� �����
void DiagramObject::changeStateLifeLineStop(int last_arrow_h)
{
	is_life_line_stop = !is_life_line_stop;
	
	//���� ����������
	if(is_life_line_stop)
		setLifeLineLen(last_arrow_h - height + life_line_len_default);
}

//������, ����������� �� ����� �����
bool DiagramObject::isLifeLineStop() const
{
	return is_life_line_stop;
}

//��������� ������� � ����
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

//��������� ������� �� �����
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

//�������, ������� ������ ����� �������� �������� � ���������� ��������� �� ���
/*virtual*/ DiagramItem* DiagramObject::getCopy() const
{
	//������� ����� �������
	DiagramObject *new_object = new DiagramObject();

	//������ ��������� �� ���� ���� DiagramItem
	DiagramItem *new_item = static_cast<DiagramItem *>(new_object);

	//������ ��������� �� ������� ������ ���� DiagramItem
	const DiagramItem * const cur_item = static_cast<const DiagramItem * const>(this);

	//�������� ��������� �� ������������� ������
	new_item->setAllParamFromOtherItem(cur_item);

	//�������� ��������� �� �������� ������
	new_object->life_line_len = life_line_len;
	new_object->is_life_line_stop = is_life_line_stop;

	return new_item;
}

//�������� ���������� ��� ������
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

//����������� ������ �� ������ ��� �������� ��������
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