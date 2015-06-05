#ifndef DIAGRAMLOOP_H
#define DIAGRAMLOOP_H

#include <QtGui>

#include "diagramobject.h"

/*
���������� ���������
*/

class DiagramLoop : public DiagramItem
{
public:
	//������������/�����������
	DiagramLoop(QGraphicsItem *parent = 0);
	~DiagramLoop();

	QPointF leftVertex;				//����� ������� ������� �������������� �����
	QPointF rightVertex;			//������ ������ �������
	QPointF beginVertex;			//�����, � ������� ���������� ���������(��������������� ��������)

	int loopTextCommentAreaWidth;	//����� ������� ���������� �����������
	int loopTextCommentAreaHeight;	//������ ������� ���������� �����������

	bool sizeDetermined;			//����, ����������� �� ��, ��� ������ ����� ��� ���������

	DiagramLoop *parentLoop;

	vector<DiagramLoop *> stacked_loops;		//��������� ����� �� ������ ������

	//����������� ����
	void move_loop(int dx, int dy, set<int> &used);

	//�������� ������ ��������� ������
	vector<DiagramLoop *> getLoopInVector(vector<QGraphicsItem *> items);

	//������ �� ���� other � �������
	bool isStacked(DiagramLoop *other) const;


	//������������ �������, ������� ����� ��������������
	//�������, ������������ ��������������� ������� �������������� ���������
	virtual QRectF boundingRect() const;
	//�������, ����������� ��������� ��������
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//�������, ������� ������ ����� �������� �������� � ���������� ��������� �� ���
	virtual DiagramItem* getCopy() const;

	//void setTextComment(QString s);														//���������� ��������� �����������
	virtual bool saveToFile(QDataStream &stream);											//��������� ������� � ����
	virtual bool loadFromFile(QDataStream &stream);											//��������� ������� �� �����
	//�������, ������� ���������� �������������� (���� ����� �������������)
	//�������, ������������ ������ ������� ��������
	virtual QPainterPath shape() const;

	virtual QByteArray getInfoForBuffer();							//�������� ���������� ��� ������
	virtual void setInfoFromBuffer(QList<QByteArray> list);			//����������� ������ �� ������ ��� �������� ��������

private:
	//������ ��� ������ � ��������
	//void updateTextCommentPos(QString s);		//�������� ������� ���������� �����������

	//��������������� ������
	QGraphicsItem * getPtrToObject(int id, const vector<QGraphicsItem *> &items) const;		//�������� ��������� �� ������
protected:
	//�������
	//virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);				//������� ����������� ������ ����
	//virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);		//������� �������� ����� �����
};

#endif // DIAGRAMOBJECT_H
