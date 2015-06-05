#ifndef DIAGRAMOBJECT_H
#define DIAGRAMOBJECT_H

#include "diagramitem.h"

/*
������ (������� ��������� ������������������)
*/

class DiagramObject : public DiagramItem
{
public:
	//������������/�����������
	DiagramObject(QGraphicsItem *parent = 0);
	~DiagramObject();

	//������������ �������, ������� ����� ��������������
	//�������, ������������ ��������������� ������� �������������� ���������
	virtual QRectF boundingRect() const;
	//�������, ����������� ��������� ��������
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//�������, ������� ������ ����� �������� �������� � ���������� ��������� �� ���
	virtual DiagramItem* getCopy() const;

	//�������, ������� ���������� �������������� (���� ����� �������������)
	//�������, ������������ ������ ������� ��������
	virtual QPainterPath shape() const;

	//������ ��� ������ � ��������
	int getLifeLineLen() const;										//������ ����� ����� �����
	bool isLifeLineStop() const;									//������, ����������� �� ����� �����
	void setLifeLineLen(int val);									//������ ����� ����� �����
	void setActivity(vector<pair<int, int> > a);					//���������� ����������
	void changeStateLifeLineStop(int last_arrow_h);					//�������� ��������� ��������� ����� �����
	virtual void setVerticalSpace(int val);							//������ ������, �� ������� ���������� ������� ���������
	virtual bool saveToFile(QDataStream &stream);					//��������� ������� � ����
	virtual bool loadFromFile(QDataStream &stream);					//��������� ������� �� �����
	virtual QByteArray getInfoForBuffer();							//�������� ���������� ��� ������
	virtual void setInfoFromBuffer(QList<QByteArray> list);			//����������� ������ �� ������ ��� �������� ��������
	
private:
	//������ �������
	vector<pair<int, int> > activity;			//����������

	//�������� �������
	int life_line_len;							//����� ����� �����
	bool is_life_line_stop;						//���� ��������� ����� �����
};

#endif // DIAGRAMOBJECT_H
