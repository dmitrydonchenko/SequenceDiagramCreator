#ifndef DIAGRAMARROW_H
#define DIAGRAMARROW_H

#include <QtGui>

#include "diagramobject.h"

/*
���������� ���������
*/

class DiagramArrow : public DiagramItem
{
public:
	//������������/�����������
	DiagramArrow(QGraphicsItem *parent = 0);
	~DiagramArrow();

	//������������ �������, ������� ����� ��������������
	//�������, ������������ ��������������� ������� �������������� ���������
	virtual QRectF boundingRect() const;
	//�������, ����������� ��������� ��������
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//�������, ������� ������ ����� �������� �������� � ���������� ��������� �� ���
	virtual DiagramItem* getCopy() const;
	
	//������ ��� ������ � ��������
	void refreshValues();																							//����������� �������� �����, ������� Drag and Drop � �������
	virtual bool saveToFile(QDataStream &stream);																	//��������� ������� � ����
	virtual bool loadFromFile(QDataStream &stream, const vector<QGraphicsItem *> &items);							//��������� ������� �� �����
	virtual QByteArray getInfoForBuffer();																			//�������� ���������� ��� ������
	virtual void setInfoFromBuffer(QList<QByteArray> list, vector<QGraphicsItem *> items, map<int, int> &new_id);	//����������� ������ �� ������ ��� �������� ��������

	//������ ���������
	QGraphicsItem *sourceItem;				//������
	QGraphicsItem *targetItem;				//����
	bool isCreationMessageArrow;			//���� ������� �������� ���������� ��������

private:
	//��������������� ������
	QGraphicsItem * getPtrToObject(int id, const vector<QGraphicsItem *> &items) const;		//�������� ��������� �� ������

protected:
};

#endif // DIAGRAMOBJECT_H
