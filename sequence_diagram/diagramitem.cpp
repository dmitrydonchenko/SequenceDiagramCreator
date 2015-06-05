#include "diagramitem.h"

DiagramItem::DiagramItem(QGraphicsItem *parent)
	: QGraphicsItem(parent)
{
	//������������� ��������� �����
	setTextComment("default");

	//������������� ������� �� ���������
	width = 100;
	height = 50;

	//������������� ���� - ������ ����� ��������
	setFlag(ItemIsSelectable);

	//������������� ����� �������
	id = -1;
	item_type = DEFAULT_ITEM;
}

DiagramItem::~DiagramItem()
{
}

//�������, ����������� ��������� ��������
/*virtual*/ void DiagramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QString s = text_item_string;

	//���� ����� ��������� ������ ��� ����������
	if(s.size() > 10)
	{
		//��������� ������ �� ������ �������� � ������ ���������
		s = s.left(10);
		s += "...";
	}

	//������ �����
	painter->drawText(text_pos_x, text_pos_y, s);
}

//���������� ������ �������
void DiagramItem::setWidth(int w)
{
	//��������� ������ ��������
	if(w <= 0) w = 1;

	//������ ����� ��������
	width = w;
}

//���������� ������ �������
void DiagramItem::setHeight(int h)
{
	//��������� ������ ��������
	if(h <= 0) h = 1;

	//������ ����� ��������
	height = h;
}

//���������� ������� �������
void DiagramItem::setSize(int w, int h)
{
	//������ ����� ��������
	setWidth(w);
	setHeight(h);
}

//�������� ������ �������
int DiagramItem::getWidth() const
{
	return width;
}

//�������� ������ �������
int DiagramItem::getHeight() const
{
	return height;
}

//�������, ������������ ������ ������� ��������
/*virtual*/ QPainterPath DiagramItem::shape() const
{
	return QGraphicsItem::shape();
}

//���������� ��������� �������
void DiagramItem::setPos(qreal x, qreal y)
{
	QGraphicsItem::setPos(x, y);
}

//���������� ��������� ������� �� ��� X
void DiagramItem::setPosX(qreal x)
{
	setPos(x, QGraphicsItem::pos().y());
}

//���������� ��������� ������� �� ��� Y
void DiagramItem::setPosY(qreal y)
{
	setPos(QGraphicsItem::pos().x(), y);
}

//�������� ��������� ����������� ��������
QString DiagramItem::getTextItemString() const
{
	return text_item_string;
}

//���������� ��������� �����������
/*virtual*/ void DiagramItem::setTextComment(QString s)
{
	//���� ������ ������ - ��������� ���������� ������
	if(s == "")
		return;

	//��������� ��������� �����������
	text_item_string = s;

	//��������� ����������� ���������
	this->setToolTip(s);

	//������������� ������� �������
	updateTextCommentPos(s);
}

//�������� ������� ���������� �����������
/*virtual*/ void DiagramItem::updateTextCommentPos(QString s)
{
	QString str = s;

	//���� ����� ��������� ������ ��� ����������
	if(str.size() > 10)
	{
		//��������� ������ �� ������ �������� � ������ ���������
		str = s.left(10);
		str += "...";
	}
	//��������� ������ � ������ ������
	int w = QFontMetrics(QGraphicsTextItem().font()).width(str);
	int h = QFontMetrics(QGraphicsTextItem().font()).height();

	//���������� � ������ �������
	text_pos_x = width / 2 - w / 2 - w / s.size() + 1;
	text_pos_y = height / 2 - h / 2;
}

//������ ������, �� ������� ���������� ������
/*virtual*/ void DiagramItem::setVerticalSpace(int val)
{
	//��������� ������ ��������
	if(val < 0) val = 0;

	//������ ����� ��������
	vertical_space = val;

	//��������� ����� �������
	setPos(pos().x(), vertical_space);

	//��������
	update();
}

//������ �� ����� ������ ��������� ������� ���������
int DiagramItem::getVerticalSpace() const
{
	return vertical_space;
}

//��������� ������� � ����
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

//��������� ������� �� �����
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

//��������� ��� ��������
bool DiagramItem::saveItemType(QDataStream &stream)
{
	stream << int(item_type);
	if(stream.status() != QDataStream::Ok)
		return false;
	return true;
}

//��������� ��� ��������
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

//�������� ��� ��������� �� ������� ��������
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

//�������� ���������� ��� ������
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

//����������� ������ �� ������ ��� �������� ��������
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