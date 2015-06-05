#ifndef DIAGRAMCOMMENT_H
#define DIAGRAMCOMMENT_H

#include "diagramitem.h"

/*
��������� �����������
*/

class DiagramComment : public DiagramItem
{
public:
	// ������������/�����������
	DiagramComment(QGraphicsItem *parent = 0);
	~DiagramComment();

	//�������, ������������ ��������������� ������� �������������� ���������
	virtual QRectF boundingRect() const;
	//�������, ����������� ��������� ��������
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//�������, ������� ������ ����� �������� �������� � ���������� ��������� �� ���
	virtual DiagramItem* getCopy() const;

	//������ ��� ������ � ���������
	virtual QByteArray getInfoForBuffer();							//�������� ���������� ��� ������
	virtual void setInfoFromBuffer(QList<QByteArray> list);			//����������� ������ �� ������ ��� �������� ��������
};

#endif // DIAGRAMCOMMENT_H