#include "diagramcomment.h"

DiagramComment::DiagramComment(QGraphicsItem *parent)
	: DiagramItem(parent)
{
	//������������� ��������� �����
	setTextComment("Comment");

	//������������� ����� �������
	item_type = COMMENT_ITEM;
}

DiagramComment::~DiagramComment()
{
}

//�������, ����������� ��������� ��������
void DiagramComment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	//���������� ����� ������ �����
	QPen current_pen = painter->pen();

	//���������� ������� ������������
	QPainterPath fillArea;
	fillArea.moveTo(0, 0);
	fillArea.lineTo(width - 20, 0);
	fillArea.lineTo(width, height - 30);
	fillArea.lineTo(width, height);
	fillArea.lineTo(0, height);
	fillArea.lineTo(0, 0);

	//����������� ���
	painter->fillPath(fillArea, QBrush(QColor(255, 255, 255)));

	//������������ �����
	DiagramItem::paint(painter, option, widget);

	//������
	painter->drawLine(width - 20, 0, width, height - 30);

	//������ ����� ��������� - ���� ������ ������� - ����� ������ ������ �������������
	if(isSelected())
	{
		//�������� ������� ���������
		QPainterPath path = shape();

		// ������ ���������� �����
		painter->setPen(QPen(Qt::DashLine));

		//������������
		painter->drawPath(path);
	}
	else
	{
		painter->drawLine(0, 0, width - 20, 0);
		painter->drawLine(width, height - 30, width, height);
		painter->drawLine(width, height, 0, height);
		painter->drawLine(0, height, 0, 0);
	}

	//���������� ��� �����
	painter->setPen(current_pen);
}

//�������, ������������ ��������������� ������� �������������� ���������
QRectF DiagramComment::boundingRect() const
{
	return QRectF(0, 0, width, height);
}

//�������, ������� ������ ����� �������� �������� � ���������� ��������� �� ���
/*virtual*/ DiagramItem* DiagramComment::getCopy() const
{
	//������� ����� �������
	DiagramComment *new_comment = new DiagramComment();

	//������ ��������� �� ���� ���� DiagramItem
	DiagramItem *new_item = static_cast<DiagramItem *>(new_comment);

	//������ ��������� �� ������� ������ ���� DiagramItem
	const DiagramItem * const cur_item = static_cast<const DiagramItem * const>(this);

	//�������� ��������� �� ������������� ������
	new_item->setAllParamFromOtherItem(cur_item);

	//�������� ��������� �� �������� ������
	//�� ��� :(

	return new_item;
}

//�������� ���������� ��� ������
/*virtual*/ QByteArray DiagramComment::getInfoForBuffer()
{
	QByteArray buf("");
	buf.append("<diagramcomment_tag>\t");
	buf.append(DiagramItem::getInfoForBuffer() + "\t");
	buf.append("</diagramcomment_tag>");
	return buf;
}

//����������� ������ �� ������ ��� �������� ��������
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